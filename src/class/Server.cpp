/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:36:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/04/30 17:51:19 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include <ostream>
#include "Server.hpp"
#include <string.h>
#include <unistd.h>
#include <cstring>




Server::Server(void) 
: _serverSocketFd(-1)
, _clientSocketFd(-1)
{
	std::memset(&this->_severSocketAdresse, 0, sizeof(sockaddr_in));
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
	return;
}

Server& Server::operator=(const Server& rhs)
{
	(void) rhs;
	return (*this);
}

int Server::getServerSocket() const 
{ return (_serverSocketFd); }

int Server::getClientSocket() const 
{ return (_clientSocketFd); }



// std::ostream & operator<<(std::ostream & out, const Server& in)
// {
// 	out << "The value of _foo is : " << in.getFoo();
// 	return (out);
// }

