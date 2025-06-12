/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.api.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/10 17:27:08 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>

#include "SocketManager.hpp"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/WriteCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/CloseCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../ConfigFile/Parser.hpp"
#include "../ConfigFile/Lexer.hpp"
#include "../ConfigFile/ConfigValidator.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"
#include <signal.h>

extern volatile sig_atomic_t g_stop;

void SocketManager::init_connect()
{
    std::ostringstream		oss;
	IVSCMAP::const_iterator	it;
    int						epoll_fd;
    int						socket_fd;
	int						port;

    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to create epoll instance", __FUNCTION__);
    _EpollFd = epoll_fd;
    for (it = _serversByPort.begin(); it != _serversByPort.end(); ++it)
    {
        port = it->first;

        ListeningSocket socket;
        if (!socket.safeBind(port, "0.0.0.0"))
        {
            oss << "Failed to bind on port " << port;
            close(socket.getFd());
            THROW_ERROR(CRITICAL, SOCKET_ERROR, oss.str(), __FUNCTION__);
        }

        socket.safeListen(128);
        socket_fd = socket.getFd();

        safeRegisterToEpoll(epoll_fd, socket_fd);

        _listeningSockets.push_back(socket);
        
        oss << "Socket listening on port " << port << std::endl;
        // std::cout << oss.str();
        LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
        oss.str("");
        oss.clear();
    }
    eventLoop(epoll_fd);
}

void SocketManager::eventLoop(int epoll_fd)
{
    EVENT_LIST	events;
    int         checkIntervalMs;
    int         n;

    events = EVENT_LIST(MAXEVENTS);
    checkIntervalMs = getCheckIntervalMs();

    while (!g_stop)
    {
        /* 1) Wait up to CHECK_INTERVAL_MS for any I/O */
        n = epoll_wait(epoll_fd, &events[0], MAXEVENTS, checkIntervalMs);
        // std::cout << "N =" << n << std::endl;
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", __FUNCTION__);
        }
        scanClientTimeouts(epoll_fd);
        _callbackQueue.processCallbacks();
        enqueueReadyCallbacks(n, events, epoll_fd);
    }
}

void SocketManager::addClientSocket(int fd, ClientSocket* client) { _clientSockets[fd] = client; }

bool SocketManager::isListeningSocket(int fd) const
{
    for (size_t i = 0; i < _listeningSockets.size(); ++i)
    {
        if (_listeningSockets[i].getFd() == fd)
            return true;
    }
    return false;
}


void SocketManager::enqueueReadyCallbacks(int n, EVENT_LIST& events, int epoll_fd)
{
    int            i;
    int            fd;
    uint32_t    ev;

    i = 0;
    while (i < n)
    {
        fd = events[i].data.fd;
        ev = events[i].events;

        if ((ev & EPOLLIN) && isListeningSocket(fd))
        {
            _callbackQueue.push(new AcceptCallback(fd, this, epoll_fd));
        }
        else if ((ev & EPOLLIN))
        {
            _callbackQueue.push(new ReadCallback(fd, this, epoll_fd));
        }
        else if ((ev & EPOLLOUT && !_waitinglist[fd].getQueue().empty()))
        {
            Callback *topush = _waitinglist[fd].getQueue().front();
            _callbackQueue.push(topush);
            _waitinglist[fd].getQueue().pop();
        }
        else if (ev & (EPOLLERR))
        {
            _callbackQueue.push(new ErrorCallback(fd, this, epoll_fd, FATAL));
        }
        else if (ev & (EPOLLHUP))
        {
            _callbackQueue.push(new CloseCallback(fd, this, epoll_fd));
        }
        i++;
    }
}

void SocketManager::scanClientTimeouts(int epoll_fd)
{
    time_t				now;
    ICMAP::iterator		it;
    int					fd;
    ClientSocket*		c;
    std::ostringstream	oss;

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
            LOG_ERROR(INFO, SOCKET_ERROR, oss.str(), __FUNCTION__);
            _callbackQueue.push(new TimeoutCallback(fd, this, epoll_fd));
        }
        ++it;
    }
}

void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    ICMAP::iterator it;
    std::ostringstream	oss;

    // 1) Deregister from epoll if applicable
    if (epoll_fd >= 0)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        {
            // Log error but continue cleanup
            oss << "epoll_ctl DEL failed for fd " << fd << ": " << strerror(errno) << std::endl;
			LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, oss.str(), __FUNCTION__);
        }
    }
    // 2) Delete the ClientSocket object
    it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        // std::cout << "CLOSE[FD = " <<  it->first << "]" << std::endl;
        close(it->first);
        delete it->second;
        _clientSockets.erase(it);
    }
    
}

IVSCMAP SocketManager::ReadandParseConfigFile(const std::string& content)
{
    Lexer  lexi(content);
    Parser configData(lexi);
    IVSCMAP configs;

    configs = configData.parseConfigFile();
    return (configs);
}



void SocketManager::instantiateConfig(const std::string& content)
{
	ConfigValidator	inspecteurGadget;
	IVSCMAP::iterator it;

    _serversByPort = ReadandParseConfigFile(content);
    if (_serversByPort.empty())
    {
        throw std::runtime_error("Empty config");   
    }
        
	for (it = _serversByPort.begin(); it != _serversByPort.end(); ++it)
	{
		inspecteurGadget.validate(it->second);        
	}
    
}
