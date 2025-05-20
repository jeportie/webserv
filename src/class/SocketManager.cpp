/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <iostream>
#include <ostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include "SocketManager.hpp"
#include "SocketCallbacks.hpp"
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"

SocketManager::SocketManager(void)
: _serverSocketFd(-1)
{
}

SocketManager::~SocketManager(void)
{
    // Clean up client sockets
    for (std::map<int, ClientSocket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end();
         ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}

void SocketManager::init_connect(void)
{
    // Create, bind, and listen on the server socket
    if (!_serverSocket.safeBind(PORT, ""))
        THROW_ERROR(CRITICAL, SOCKET_ERROR, "Failed to bind server socket",
					"SocketManager::init_connect");

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

void SocketManager::eventLoop(int epoll_fd, int timeout_ms)
{
    std::vector<epoll_event> events(MAXEVENTS);
    bool running = true;

    while (running)
    {
        processTimers();
        _callbackQueue.processCallbacks();
        int wait_timeout = this->calculateEpollTimeout(timeout_ms);

        // Wait for events
        int n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, wait_timeout);
        if (n < 0)
        {
            if (errno == EINTR)
                continue;
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", "SocketManager::eventLoop");
        }

        // Process events by queueing appropriate callbacks
        for (int i = 0; i < n; ++i)
        {
            int      fd = events[i].data.fd;
            uint32_t ev = events[i].events;

            if ((ev & EPOLLIN) && fd == _serverSocketFd)
            {
                // New connection on the server socket
                _callbackQueue.push(new AcceptCallback(fd, this, epoll_fd));
            }
            else if ((ev & EPOLLIN) && fd != _serverSocketFd)
            {
                // Data available on a client socket
                _callbackQueue.push(new ReadCallback(fd, this));
            }
            else if (ev & (EPOLLERR | EPOLLHUP))
            {
                // Error or hangup on a socket
                _callbackQueue.push(new ErrorCallback(fd, this, epoll_fd));
            }
        }
    }
}

int SocketManager::calculateEpollTimeout(int timeout_ms)
{
    if (_timerQueue.empty())
        return timeout_ms;
        
    time_t now = time(NULL);
    time_t next_expire = _timerQueue.top().getExpireTime();
    
    if (next_expire <= now)
        return 0;  // Process immediately
        
    int wait_timeout = (next_expire - now) * 1000;  // Convert to milliseconds
    
    if (timeout_ms != -1 && wait_timeout > timeout_ms)
        return timeout_ms;  // Use the smaller timeout
        
    return wait_timeout;
}

void SocketManager::addClientSocket(int fd, ClientSocket* client) { _clientSockets[fd] = client; }

ServerSocket& SocketManager::getServerSocket() { return _serverSocket; }

CallbackQueue& SocketManager::getCallbackQueue() { return _callbackQueue; }

void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    // Cancel any timers for this client
    cancelTimer(fd);

    // Clean up the client socket
    if (fd != _serverSocketFd)
    {
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        // Remove from epoll
        if (epoll_fd >= 0)
        {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        }
        close(fd);
    }
}

int SocketManager::addTimer(int seconds, Callback* callback)
{
    time_t expireTime = time(NULL) + seconds;
    Timer  timer(expireTime, callback);
    _timerQueue.push(timer);
    return callback->getFd();  // Use the file descriptor as the timer ID
}

bool SocketManager::cancelTimer(int fd)
{
    // We can't easily remove from a priority queue, so we'll mark the callback as cancelled
    std::priority_queue<Timer> temp;
    bool                       found = false;

    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();
        _timerQueue.pop();

        Callback* callback = timer.getCallback();
        if (callback->getFd() == fd)
        {
            // In our simplified model, we just delete the callback
            delete callback;
            found = true;
        }
        else
        {
            temp.push(timer);
        }
    }

    _timerQueue = temp;
    return (found);
}

void SocketManager::processTimers()
{
    time_t now = time(NULL);

    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();

        if (timer.getExpireTime() > now)
        {
            // No more expired timers
            break;
        }

        // Remove the timer from the queue
        _timerQueue.pop();

        // Execute the callback
        Callback* callback = timer.getCallback();
        _callbackQueue.push(callback);
    }
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
