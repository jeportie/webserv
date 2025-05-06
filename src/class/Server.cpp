/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:36:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/06 15:10:44 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "exception"
#include <cstring>
#include <iostream>
#include <ostream>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

#define PORT 8666
#define BUFFER_SIZE 1024

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
	if (listen(_serverSocketFd, 1) < 0)
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
	epoll_event client_ev;
	client_ev.events = EPOLLIN;
	client_ev.data.fd = _clientSocketFd;
	epoll_ctl(epoll_fd , EPOLL_CTL_ADD, _clientSocketFd, &client_ev);
	return (true);
}

void Server::init_connect(void)
{

		safeBind();
		safeListen();
		int epoll_fd = epoll_create(1);
		epoll_event ev;
		ev.events = EPOLLIN; // On veut savoir quqnd on peutr read()'
		ev.data.fd = _serverSocketFd; 
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev))
		safeAccept(epoll_fd);
}

void Server::communication()
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;

	bytes_read = -1;
	// bytes_read = read(_clientSocketFd, buffer, BUFFER_SIZE - 1);
	while (bytes_read)
	{
		std::memset(buffer, 0, BUFFER_SIZE);
		bytes_read = read(_clientSocketFd, buffer, BUFFER_SIZE - 1);
		std::cout << buffer;
		write(_clientSocketFd, "Re", 2);
	}
}
// std::ostream & operator<<(std::ostream & out, const Server& in)
// {
// 	out << "The value of _foo is : " << in.getFoo();
// 	return (out);
// }

/* Pourquoi deux sockets ?

   listen_fd : toujours en écoute,
	ne transmet jamais directement de données clients.

   conn_fd : dédié à un seul client, sert à échanger requêtes/réponses.
   */
