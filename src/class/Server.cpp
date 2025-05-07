/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:36:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 12:28:51 by fsalomon         ###   ########.fr       */
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
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>


Server::Server(void)
: _serverSocketFd(-1)
, _clientSocketFd(-1)
{
    std::memset(&this->_serverSockAdr, 0, sizeof(sockaddr_in));
    std::cout << "[Server] - default constructor called - " << std::endl;
}

Server::Server(const Server& src)
{
    *this = src;
    return;
}

Server::~Server(void)
{
    std::cout << "[Server] - destructor called - " << std::endl;
    close(_serverSocketFd);
    close(_clientSocketFd);
    return;
}

Server& Server::operator=(const Server& rhs)
{
    (void) rhs;
    return (*this);
}

int Server::getServerSocket() const { return (_serverSocketFd); }

int Server::getClientSocket() const { return (_clientSocketFd); }


void Server::eventLoop(int epoll_fd)
{
    epoll_event events[MAXEVENTS];

    while (true)
    {
        int n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        if (n < 0)
            throw std::runtime_error("epoll_wait failed!");
        for (int i = 0; i < n; ++i)
        {
            int      fd = events[i].data.fd;
            uint64_t ev = events[i].events;

            if ((ev & EPOLLIN) && fd == _serverSocketFd)
                safeAccept(epoll_fd);
            else if ((ev & EPOLLIN) && fd != _serverSocketFd)
                communication(fd);
            else if (ev & EPOLLERR)
                close(fd);
        }
    }
}

void Server::init_connect(void)
{
    int         epoll_fd = epoll_create(1);
    
    safeBind();
    safeListen();
    safeRegisterToEpoll(epoll_fd);
    eventLoop(epoll_fd);
}

void Server::communication(int fd)
{
    char buffer[BUFFER_SIZE];
    int  bytes_read;

    bytes_read = -1;
    while ((bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytes_read] = '\0';
        std::cout << buffer;
    }
    if (bytes_read == 0)
    {
        // Le client a fermé proprement la connexion
        close(fd);
    }
    else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // Erreur de lecture inattendue
        perror("read");
        close(fd);
    }
    // Si errno == EAGAIN : on sort et on attend le prochain EPOLLIN
}

/* Pourquoi deux sockets ?

   listen_fd : toujours en écoute,
    ne transmet jamais directement de données clients.

   conn_fd : dédié à un seul client, sert à échanger requêtes/réponses.
   */
