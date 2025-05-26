/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.api.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/26 12:35:09 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <cstdlib>

#include "SocketManager.hpp"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../ConfigFile/Parser.hpp"
#include "../ConfigFile/Lexer.hpp"
#include "../ConfigFile/ConfigValidator.hpp"
#include "../../../include/webserv.h"

void SocketManager::init_connect(const IVSCMAP& serversByPort)
{
    std::ostringstream	oss;
    int					epoll_fd;

    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to create epoll instance", __FUNCTION__);

    _epollFd = epoll_fd;

    for (IVSCMAP::const_iterator it = serversByPort.begin(); it != serversByPort.end(); ++it)
    {
        int port = it->first;

        // Créer et binder le socket
        ServerSocket socket;
        if (!socket.safeBind(port, "0.0.0.0"))  // ou it->second[0].host si tu veux gérer par IP
        {
            oss << "Failed to bind on port " << port;
            THROW_ERROR(CRITICAL, SOCKET_ERROR, oss.str(), __FUNCTION__);
        }

        socket.safeListen(10);
        int socket_fd = socket.getFd();

        // Enregistrer dans ton epoll
        safeRegisterToEpoll(epoll_fd, socket_fd);

        // Stocker les sockets + configs (pour plus tard)
        _serverSockets.push_back(socket); // si tu as un vecteur
        _configuration[port] = it->second; // map<int, vector<ServerConfig>>

        // Log
        oss << "Server listening on port " << port << std::endl;
        std::cout << oss.str();
        LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
        oss.str(""); // reset pour la prochaine boucle
        oss.clear();
    }

    // Lance la boucle principale
    eventLoop(epoll_fd);
}

void SocketManager::init_connect(void)
{
	std::ostringstream	oss;
	int					epoll_fd;

    // Create, bind, and listen on the server socket

    //while nombre de server 
    if (!_serverSocket.safeBind(PORT, ""))
	{
        THROW_ERROR(CRITICAL, SOCKET_ERROR, "Failed to bind server socket", __FUNCTION__);
	}
    _serverSocket.safeListen(10);
    _serverSocketFd = _serverSocket.getFd();

    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
	{
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to create epoll instance", __FUNCTION__);
	}
    safeRegisterToEpoll(epoll_fd);

	oss << "Server listening on port " << PORT << std::endl;
	std::cout << oss.str();
	LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
	
    eventLoop(epoll_fd);
}

void SocketManager::eventLoop(int epoll_fd)
{
    EVENT_LIST	events;
    int         checkIntervalMs;
    bool		running;
    int         n;

    events = EVENT_LIST(MAXEVENTS);
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
    for (size_t i = 0; i < _serverSockets.size(); ++i)
    {
        if (_serverSockets[i].getFd() == fd)
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
        else if (ev & (EPOLLERR | EPOLLHUP))
        {
            _callbackQueue.push(new ErrorCallback(fd, this, epoll_fd));
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

     _configuration = ReadandParseConfigFile(content);
	for (it = _configuration.begin(); it != _configuration.end(); ++it)
	{
		inspecteurGadget.validate(it->second);
	}
}
