/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:32:17 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 16:00:59 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/socket.h>

Socket::Socket(void) {}

Socket::~Socket(void) { return; }

Socket::Socket(const Socket& src)
{
    *this = src;
    return;
}

Socket& Socket::operator=(const Socket& rhs)
{
    //	if (this != &rhs)
    //	{
    //
    //	}
    return (*this);
}

bool Socket::socketCreate(void)
{
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
        throw std::runtime_error("Socket() Failed");
    return (true);
}

// to implement:
bool Socket::setReuseAddr(bool reuse) { return (true); }

int Socket::getFd(void) const { return (_socketFd); }

void Socket::setFd(int fd) { _socketFd = fd;}

bool Socket::isValid(void) const { return (_socketFd != -1); }

bool Socket::isNonBlocking(void) const { return (_isNonBlocking); }

void Socket::closeSocket(void)
{ 
	if (isValid())
		close(_socketFd);
}
