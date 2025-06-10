/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/10 15:59:58 by anastruc         ###   ########.fr       */
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
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"
#include <iostream>

// Constrctor & Destructor
SocketManager::SocketManager(void)
: _clientSocketFd(-1), _EpollFd(-1)
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

std::vector<ListeningSocket>& SocketManager::getVectorListeningSocket()
{
    return _listeningSockets;
}

ListeningSocket* SocketManager::getListeningSocket(int fd)
{
    for (size_t i = 0; i < _listeningSockets.size(); ++i)
    {
        if (_listeningSockets[i].getFd() == fd)
            return &_listeningSockets[i];
    }
    return (NULL);
}


CallbackQueue& SocketManager::getCallbackQueue() { return _callbackQueue; }

int SocketManager::getClientSocketFd(void) const { return (_clientSocketFd); }

int SocketManager::getEpollFd(void) const { return (_EpollFd); }


// IVSCMAP SocketManager::getConfiguration(void) const is now inline in the header

int SocketManager::setNonBlockingListening(int fd)
{
    // Find the listening socket with this fd and set it to non-blocking
    for (size_t i = 0; i < _listeningSockets.size(); ++i)
    {
        if (_listeningSockets[i].getFd() == fd)
            return _listeningSockets[i].setNonBlocking(fd);
    }
    return -1;  // Socket not found
}

// Safe Wrappers
void SocketManager::safeRegisterToEpoll(int epoll_fd, int listeningFd)
{
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = listeningFd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listeningFd, &ev) == -1)
    {
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, LOG_SM_EPOLL, __FUNCTION__);
    }
}

int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    std::ostringstream oss;

    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        oss << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd << ",op=" << op
            << "): " << strerror(errno) << std::endl;
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, oss.str(), __FUNCTION__);
        return (-1);
    }
    return (0);
}


ICMAP& SocketManager::getClientMapNonConst(void) { return (_clientSockets); }
