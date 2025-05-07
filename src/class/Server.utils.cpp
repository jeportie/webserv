/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:28:05 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 12:32:29 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../../include/webserv.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>


int safeFcntlServer(int fd, int cmd, int flag)
{
	int ret = fcntl(fd, cmd, flag);
	if (ret == -1)
		throw std::runtime_error("fcntl server socket failed");
	return ret;
    
}

int safeFcntlClient(int fd, int cmd, int flag)
{
    int ret = fcntl(fd, cmd, flag);
    if (ret == -1)
    {
        std::cerr << "[Error] fcntl failed on client fd " << fd
                  << ": " << strerror(errno) << std::endl;
        return -1;
    }
    return ret;
}
    
        
int Server::setNonBlockingServer(int fd)
{
    int flags = safeFcntlServer(fd, F_GETFL, 0);
    return (safeFcntlServer(fd, F_SETFL, flags | O_NONBLOCK));
}

int setNonBlockingClient(int fd)
{
    int flags = safeFcntlClient(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    //Futur gestion erreur client a implementer ici
    return(safeFcntlClient(fd, F_SETFL, flags | O_NONBLOCK));
}



void Server::safeBind(void)
{
    setNonBlockingServer(_serverSocketFd);
    _serverSockAdr.sin_port = htons(PORT);
    _serverSockAdr.sin_family      = AF_INET; 
    _serverSockAdr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_serverSocketFd, (struct sockaddr*) &_serverSockAdr, sizeof(_serverSockAdr)) < 0)
        throw std::runtime_error("Bind() Failed");
}


void Server::safeListen(void)
{
    if (listen(_serverSocketFd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen() Failed");
}


int safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd
                  << ", fd=" << fd << ", op=" << op << "): "
                  << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}

void Server::safeRegisterToEpoll(int epoll_fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _serverSocketFd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
		throw std::runtime_error("Failed to add server socket to epoll: " + std::string(strerror(errno)));
}
void Server::safeAccept(int epoll_fd)
{
    _clientSocketFd = accept(_serverSocketFd, (struct sockaddr*) &_clientSockAdr, &_clientSockAdrLen);
    if (_clientSocketFd < 0)
        throw std::runtime_error("Accept() Failed");
        
    std::cout << "Connection accepted!\n";
    setNonBlockingClient(_clientSocketFd);
    
    epoll_event client_ev;
    client_ev.events  = EPOLLIN | EPOLLET;
    client_ev.data.fd = _clientSocketFd;
    
    safeEpollCtlClient(epoll_fd, EPOLL_CTL_ADD, _clientSocketFd, &client_ev);
}
