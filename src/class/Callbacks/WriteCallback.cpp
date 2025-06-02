/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:56:33 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:53:23 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"
#include "WriteCallback.hpp"
#include "ErrorCallback.hpp"
#include "ReadCallback.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// WriteCallback implementation
WriteCallback::WriteCallback(int clientFd, SocketManager* manager, const std::string& data, int epoll_fd)
: Callback(clientFd), _manager(manager), _data(data), _epoll_fd(epoll_fd)
{
	LOG_ERROR(DEBUG, CALLBACK_ERROR, "WriteCallback Constructor called.", __FUNCTION__);
}

WriteCallback::~WriteCallback()
{
	LOG_ERROR(DEBUG, CALLBACK_ERROR, "WriteCallback Destructor called.", __FUNCTION__);
}

void WriteCallback::execute()
{
    ssize_t written;
    std::ostringstream oss;
    struct epoll_event ev;

    // Modify the socket to be monitored for write events
    ev.events = EPOLLOUT;
    ev.data.fd = _fd;
    
    if (_manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev) < 0)
    {
        LOG_SYSTEM_ERROR(ERROR, EPOLL_ERROR, "Failed to modify socket for write events", __FUNCTION__);
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _epoll_fd));
        return;
    }

    // Write the data to the client
    written = write(_fd, _data.c_str(), _data.length());
    
    if (written < 0)
    {
        LOG_SYSTEM_ERROR(WARNING, SOCKET_ERROR, "Failed to write response to client", __FUNCTION__);
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _epoll_fd));
        return;
    }
    
    oss << "Successfully wrote " << written << " bytes to client (fd=" << _fd << ")";
    LOG_ERROR(DEBUG, CALLBACK_ERROR, oss.str(), __FUNCTION__);
    
    // Reset the socket to be monitored for read events
    ev.events = EPOLLIN;
    ev.data.fd = _fd;
    
    if (_manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev) < 0)
    {
        LOG_SYSTEM_ERROR(ERROR, EPOLL_ERROR, "Failed to reset socket for read events", __FUNCTION__);
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _epoll_fd));
        return;
    }
}
