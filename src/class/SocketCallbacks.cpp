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

void ReadCallback::execute()
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    bytes_read = read(_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        std::cout << "Received from client (fd=" << _fd << "): " << buffer;

        // Reset the timeout for this client
        _manager->cancelTimer(_fd);
        _manager->addTimer(CLIENT_TIMEOUT, new TimeoutCallback(_fd, _manager, -1));

        // Echo back to the client - in a real HTTP server, this would be replaced with
        // request parsing and response generation
        _manager->getCallbackQueue().push(new WriteCallback(_fd, _manager, std::string(buffer, bytes_read)));
    }
    else if (bytes_read == 0)
    {
        // Client closed the connection
        std::stringstream ss;
        ss << _fd;
        LOG_ERROR(INFO,
                  SOCKET_ERROR,
                  "Client disconnected (fd=" + ss.str() + ")",
                  "ReadCallback::execute");

        // Queue an error callback to clean up the client socket
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
    else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // Unexpected read error
        std::stringstream ss;
        ss << _fd;
        LOG_SYSTEM_ERROR(
            ERROR, SOCKET_ERROR, "Read error on fd " + ss.str(), "ReadCallback::execute");

        // Queue an error callback to clean up the client socket
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
    // If errno == EAGAIN or EWOULDBLOCK, we'll wait for the next EPOLLIN event
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

