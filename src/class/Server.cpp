/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:36:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/06 18:06:46 by anastruc         ###   ########.fr       */
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


Server::Server(void) : _serverSocketFd(-1), _clientSocketFd(-1)
{
	std::memset(&this->_serverSocketAdresse, 0, sizeof(sockaddr_in));
	std::cout << "[Server] - default constructor called - " << std::endl;
}

Server::Server(const Server &src)
{
	*this = src;
	return ;
}

Server::~Server(void)
{
	std::cout << "[Server] - destructor called - " << std::endl;
	close(_serverSocketFd);
	close(_clientSocketFd);
	return ;
}

Server &Server::operator=(const Server &rhs)
{
	(void)rhs;
	return (*this);
}

int Server::getServerSocket() const
{
	return (_serverSocketFd);
}

int Server::getClientSocket() const
{
	return (_clientSocketFd);
}
//aatenntion a reformater
int set_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool Server::safeBind(void)
{
	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	// Format IPV4, SOCKET_STREAM = TCP
	if (_serverSocketFd < 0)
	// link the socket to the local Adress (IP + Port).
	{
		throw std::runtime_error("Socket() Failed");
		return (false);
	}
	set_non_blocking(_serverSocketFd);
	_serverSocketAdresse.sin_port = htons(PORT);
	// Stock port nbr and convert it in bigindian. Host interpret port nbr in L.indian and internet interprets it in B.Indian
	_serverSocketAdresse.sin_family = AF_INET; // Format IPV4
	_serverSocketAdresse.sin_addr.s_addr = INADDR_ANY;
	// Mask 0.0.0.0 --> accept every IP.
	if (bind(_serverSocketFd, (struct sockaddr *)&_serverSocketAdresse,
			sizeof(_serverSocketAdresse)) < 0)
	// link the socket to the local Adress (IP + Port).
	{
		throw std::runtime_error("Bind() Failed");
		return (false);
	}
	return (true);
}

bool Server::safeListen(void)
// Convert the server socket to listenning mode (PASSIF);
{
	if (listen(_serverSocketFd, SOMAXCONN) < 0)
	{
		throw std::runtime_error("Listen() Failed");
		return (false);
	}

	return (true);
}


bool Server::safeAccept(int epoll_fd)
{
	_clientSocketFd = accept(_serverSocketFd,
			(struct sockaddr *)&_clientSocketAdresse,
			&_clientSocketAdresseLength);
	if (_clientSocketFd < 0)
	{
		throw std::runtime_error("Accept() Failed");
		return (false);
	}
	std::cout << "Connection accepted!\n";
	set_non_blocking(_clientSocketFd);
	epoll_event client_ev;
	client_ev.events = EPOLLIN | EPOLLET;
	client_ev.data.fd = _clientSocketFd;
	epoll_ctl(epoll_fd , EPOLL_CTL_ADD, _clientSocketFd, &client_ev);
	return (true);
}

void Server::eventLoop(int epoll_fd)
{
	std::vector<epoll_event> events(MAXEVENTS);

	while (true)
	{
		int n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, -1);
		if (n < 0)
			throw std::runtime_error("epoll_wait failed!");
		for (int i = 0; i < n; ++i)
		{
			int	fd		= events[i].data.fd;
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
	safeBind();
	safeListen();
	int epoll_fd = epoll_create(1);
	epoll_event ev;
	ev.events = EPOLLIN; // On veut savoir quqnd on peutr read()'
	ev.data.fd = _serverSocketFd; 
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) < 0)
		throw std::runtime_error("poll_ctl < 0 ! (With server socket)");
	else
		eventLoop(epoll_fd);
}

void Server::communication(int fd)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;

	bytes_read = -1;
	while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0)
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
