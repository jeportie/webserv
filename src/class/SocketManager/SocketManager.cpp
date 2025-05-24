/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/24 13:02:03 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

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
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "SocketManager Constructor called.", "SocketManager::SocketManager()");
}

SocketManager::~SocketManager(void)
{
    std::map<int, ClientSocket*>::iterator it;

    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "SocketManager Destructor called.", "SocketManager::~SocketManager()");

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

int SocketManager::setNonBlockingServer(int fd) { return (_serverSocket.setNonBlocking(fd)); }

// Safe Wrappers
void SocketManager::safeRegisterToEpoll(int epoll_fd)
{
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = _serverSocketFd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
        THROW_SYSTEM_ERROR(CRITICAL,
                           EPOLL_ERROR,
                           "Failed to add server socket to epoll",
                           "SocketManager::safeRegisterToEpoll");
}

int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd
                  << ",op=" << op << "): " << strerror(errno) << std::endl;
        return (-1);
    }
    return (0);
}
