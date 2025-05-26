/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/26 12:28:41 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sstream>

#include "SocketManager.hpp"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../../../include/webserv.h"

// Constrctor & Destructor
SocketManager::SocketManager(void)
: _serverSocketFd(-1)
, _clientSocketFd(-1)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SM_CONST, __FUNCTION__);
}

SocketManager::~SocketManager(void)
{
    std::map<int, ClientSocket*>::iterator it;

    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SM_DEST, __FUNCTION__);

    for (it = _clientSockets.begin(); it != _clientSockets.end(); ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}

// Getters & Setter
int SocketManager::getCheckIntervalMs(void) { return 1000; }

const ICMAP& SocketManager::getClientMap(void) const { return (_clientSockets); }

ServerSocket& SocketManager::getServerSocket() { return _serverSocket; }

CallbackQueue& SocketManager::getCallbackQueue() { return _callbackQueue; }

int SocketManager::getServerSocketFd(void) const { return (_serverSocketFd); }

int SocketManager::getClientSocketFd(void) const { return (_clientSocketFd); }

SSCMAP SocketManager::getConfiguration(void) const { return (_configuration); }

int SocketManager::setNonBlockingServer(int fd) { return (_serverSocket.setNonBlocking(fd)); }

// Safe Wrappers
void SocketManager::safeRegisterToEpoll(int epoll_fd)
{
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = _serverSocketFd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, LOG_SM_EPOLL, __FUNCTION__);
}

int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    std::ostringstream	oss;

    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        oss << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd
                  << ",op=" << op << "): " << strerror(errno) << std::endl;
		LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, oss.str(), __FUNCTION__);
        return (-1);
    }
    return (0);
}
