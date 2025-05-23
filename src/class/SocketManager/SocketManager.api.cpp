/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:21:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>

#include "SocketManager.hpp"
#include "../../../include/webserv.h"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Sockets/ClientSocket.hpp"

void SocketManager::init_connect(void)
{
    // Create, bind, and listen on the server socket
    if (!_serverSocket.safeBind(PORT, ""))
        THROW_ERROR(
            CRITICAL, SOCKET_ERROR, "Failed to bind server socket", "SocketManager::init_connect");

    _serverSocket.safeListen(10);
    _serverSocketFd = _serverSocket.getFd();

    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
        THROW_SYSTEM_ERROR(CRITICAL,
                           EPOLL_ERROR,
                           "Failed to create epoll instance",
                           "SocketManager::init_connect");

    safeRegisterToEpoll(epoll_fd);
    std::cout << "Server listening on port " << PORT << std::endl;
    eventLoop(epoll_fd);
}

void SocketManager::eventLoop(int epoll_fd)
{
    std::vector<epoll_event>    events;
    int                         checkIntervalMs;
    bool                        running;
    int                         n;

    events = std::vector<epoll_event>(MAXEVENTS);
    checkIntervalMs = getCheckIntervalMs();
    running = true;

    while (running)
    {
        /* 1) Wait up to CHECK_INTERVAL_MS for any I/O */
        n = epoll_wait(epoll_fd, &events[0], MAXEVENTS, checkIntervalMs);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", "SocketManager::eventLoop");
        }

        scanClientTimeouts(epoll_fd);

        _callbackQueue.processCallbacks();

        enqueueReadyCallbacks(n, events, epoll_fd);
    }
}

int SocketManager::getCheckIntervalMs(void)
{
    return 1000;
}

const std::map<int, ClientSocket*>& SocketManager::getClientMap(void) const
{
    return (_clientSockets);
}

void SocketManager::enqueueReadyCallbacks(int n, std::vector<epoll_event>& events, int epoll_fd)
{
    int i;
    int fd;
    uint32_t ev;

    i = 0;
    while (i < n)
    {
        fd = events[i].data.fd;
        ev = events[i].events;

        if ((ev & EPOLLIN) && fd == _serverSocketFd)
        {
            _callbackQueue.push(new AcceptCallback(fd, this, epoll_fd));
        }
        else if ((ev & EPOLLIN) && fd != _serverSocketFd)
        {
            _callbackQueue.push(new ReadCallback(fd, this, epoll_fd));
        }
        else if (ev & (EPOLLERR | EPOLLHUP))
        {
            _callbackQueue.push(new ErrorCallback(fd, this, epoll_fd));
        }
        i++;
    }
}

void SocketManager::scanClientTimeouts(int epoll_fd)
{
    time_t now;
    std::map<int, ClientSocket*>::iterator it;
    int fd;
    ClientSocket* c;
    std::ostringstream oss;

    now = time(NULL);
    it = _clientSockets.begin();
    while (it != _clientSockets.end())
    {
        fd = it->first;
        c = it->second;
        if (now - c->getLastActivity() >= CLIENT_TIMEOUT)
        {
            oss.str("");
            oss.clear();
            oss << "Client timed out (fd=" << fd << ")";
            LOG_ERROR(INFO, SOCKET_ERROR,
                      oss.str(),
                      "SocketManager::scanClientTimeouts");
            _callbackQueue.push(new TimeoutCallback(fd, this, epoll_fd));
        }
        ++it;
    }
}


void SocketManager::addClientSocket(int fd, ClientSocket* client) { _clientSockets[fd] = client; }

ServerSocket& SocketManager::getServerSocket() { return _serverSocket; }

CallbackQueue& SocketManager::getCallbackQueue() { return _callbackQueue; }


void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    // 1) Deregister from epoll if applicable
    if (epoll_fd >= 0)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        {
            // Log error but continue cleanup
            std::cerr << "epoll_ctl DEL failed for fd " << fd << ": " << strerror(errno)
                      << std::endl;
        }
    }

    // 2) Delete the ClientSocket object
    std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        delete it->second;
        _clientSockets.erase(it);
    }

    // // 3) Close the file descriptor
    // if (close(fd) == -1)
    // {
    //     std::cerr << "close() failed for fd " << fd << ": " << strerror(errno) << std::endl;
    // }
}


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

int SocketManager::setNonBlockingServer(int fd) { return (_serverSocket.setNonBlocking(fd)); }

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

int SocketManager::getServerSocketFd(void) const { return (_serverSocketFd); }

int SocketManager::getClientSocketFd(void) const { return (_clientSocketFd); }
