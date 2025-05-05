/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:36:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/05 19:52:42 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "exception"
#include <cstring>
#include <iostream>
#include <ostream>
#include <string.h>
#include <unistd.h>

#define PORT 8666
#define BUFFER_SIZE 1024

Server::Server(void) : _serverSocketFd(-1), _clientSocketFd(-1)
{
	std::memset(&this->_serverSocketAdresse, 0, sizeof(sockaddr_in));
	std::cout << "[Server] - default constructor called - " << std::endl;
}


Server::~Server(void)
{
	std::cout << "[Server] - destructor called - " << std::endl;
	close(_serverSocketFd);
	close(_clientSocketFd);
	return ;
}

Server::Server(const Server &src)
{
	*this = src;
	return ;
}

Server &Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		this->_serverSocketFd = rhs._serverSocketFd;
		this->_clientSocketFd = rhs._clientSocketFd;
		this->_serverSocketAdresse = rhs._serverSocketAdresse;
		this->_clientSocketAdresse = rhs._clientSocketAdresse;
		this->_clientSocketAdresseLength = rhs._clientSocketAdresseLength;
		this->_port = rhs._port;
		this->_serverName = rhs._serverName;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_errorPage = rhs._errorPage;
	}
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

int Server::getPort() const
{
	return (_port);
}

std::string Server::getServerName() const
{
	return (_serverName);
}

std::string Server::getRoot() const
{
	return (_root);
}

std::string Server::getIndex() const
{
	return (_index);
}

std::string Server::getErrorPage() const
{
	return (_errorPage);
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setServerName(std::string serverName)
{
	_serverName = serverName;
}

void Server::setRoot(std::string root)
{
	_root = root;
}

void Server::setIndex(std::string index)
{
	_index = index;
}

void Server::setErrorPage(std::string errorPage)
{
	_errorPage = errorPage;
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

bool Server::safeAccept(void)
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
	return (true);
}

void Server::connect(void)
{
	try
	{
		safeBind();
		safeListen();
		safeAccept();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
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

/* Pourquoi deux sockets ?

   listen_fd : toujours en écoute,
	ne transmet jamais directement de données clients.

   conn_fd : dédié à un seul client, sert à échanger requêtes/réponses.
   */

   
std::ostream & operator<<(std::ostream & out, const Server& in)
{
	out << "The value of port is : " << in.getPort() << std::endl;
	out << "The value of serverName is : " << in.getServerName() << std::endl;
	out << "The value of root is : " << in.getRoot() << std::endl;
	out << "The value of index is : " << in.getIndex() << std::endl;
	out << "The value of errorPage is : " << in.getErrorPage() << std::endl;
	return (out);
}
