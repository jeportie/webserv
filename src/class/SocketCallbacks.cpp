/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketCallbacks.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketCallbacks.hpp"
#include "SocketManager.hpp"
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <sstream>

// AcceptCallback implementation
AcceptCallback::AcceptCallback(int serverFd, SocketManager* manager, int epollFd)
: Callback(serverFd), _manager(manager), _epollFd(epollFd)
{
}

AcceptCallback::~AcceptCallback()
{
}

void AcceptCallback::execute()
{
    try
    {
        ClientSocket* client = _manager->getServerSocket().safeAccept(_epollFd);
        if (client)
        {
            int clientFd = client->getFd();
            _manager->addClientSocket(clientFd, client);
            
            // Add a timeout for idle connections
            _manager->addTimer(CLIENT_TIMEOUT, new TimeoutCallback(clientFd, _manager, _epollFd));
            
            std::cout << "New connection from " << client->getClientIP() << ":" 
                      << client->getClientPort() << " (fd=" << clientFd << ")" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(ERROR,
                  SOCKET_ERROR,
                  "Accept failed: " + std::string(e.what()),
                  "AcceptCallback::execute");
    }
}

// ReadCallback implementation
ReadCallback::ReadCallback(int clientFd, SocketManager* manager)
: Callback(clientFd), _manager(manager)
{
}

ReadCallback::~ReadCallback()
{
}

void ReadCallback::execute() {
    try {
        // communication() returns false if the socket should be closed
        if (!_manager->communication(_fd)) {
            std::ostringstream oss;
            oss << _fd;
            LOG_ERROR(INFO, SOCKET_ERROR, "Closing client connection (fd=" + oss.str() + ")", "ReadCallback::execute");
            _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR(ERROR, CALLBACK_ERROR, e.what(), "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    } catch (...) {
        LOG_ERROR(ERROR, CALLBACK_ERROR, "Unknown error", "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
}

// WriteCallback implementation
WriteCallback::WriteCallback(int clientFd, SocketManager* manager, const std::string& data)
: Callback(clientFd), _manager(manager), _data(data)
{
}

WriteCallback::~WriteCallback()
{
}

void WriteCallback::execute()
{
    if (write(_fd, _data.c_str(), _data.length()) < 0)
    {
        LOG_SYSTEM_ERROR(WARNING,
                         SOCKET_ERROR,
                         "Failed to write response to client",
                         "WriteCallback::execute");
        
        // Queue an error callback if write failed
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
}

// ErrorCallback implementation
ErrorCallback::ErrorCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
}

ErrorCallback::~ErrorCallback()
{
}

void ErrorCallback::execute()
{
    // Cancel any timers for this client
    _manager->cancelTimer(_fd);

    // Clean up the client socket
    _manager->cleanupClientSocket(_fd, _epollFd);
}

// TimeoutCallback implementation
TimeoutCallback::TimeoutCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
}

TimeoutCallback::~TimeoutCallback()
{
}

void TimeoutCallback::execute()
{
    std::cout << "Client connection timed out (fd=" << _fd << ")" << std::endl;
    
    // Clean up the client socket
    _manager->cleanupClientSocket(_fd, _epollFd);
}

