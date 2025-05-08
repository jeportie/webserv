/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:32:17 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:14:38 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/socket.h>
#include <iostream>
#include <cerrno>

Socket::Socket(void)
: _socketFd(-1)
, _isNonBlocking(false)
{}

Socket::~Socket(void) { closeSocket(); }

Socket::Socket(const Socket& src)
: _socketFd(-1)
, _isNonBlocking(false)
{ *this = src; }

/**
 * @brief Assignment operator
 * Note: Does not duplicate the file descriptor, as this would be unsafe
 */
Socket& Socket::operator=(const Socket& rhs)
{
    if (this != &rhs)
        _isNonBlocking = rhs._isNonBlocking;
    return (*this);
}
/**
 * @brief Creates a socket using socket() system call
 * Note: No throw here to let client or server soket deal with the error
 * @return true if socket creation was successful, false otherwise
 */
bool Socket::socketCreate(void)
{
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
    {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Sets the SO_REUSEADDR socket option
 * 
 * @param reuse Whether to enable (true) or disable (false) the option
 * @return true if successful, false otherwise
 */
bool Socket::setReuseAddr(bool reuse)
{
    if (!isValid())
    {
        std::cerr << "Cannot set SO_REUSEADDR on invalid socket" << std::endl;
        return false;
    }
    
    int option = reuse ? 1 : 0;
    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
    {
        std::cerr << "Error setting SO_REUSEADDR: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

int Socket::getFd(void) const { return (_socketFd); }

void Socket::setFd(int fd) { _socketFd = fd;}

bool Socket::isValid(void) const { return (_socketFd != -1); }

bool Socket::isNonBlocking(void) const { return (_isNonBlocking); }

void Socket::closeSocket(void)
{ 
    if (isValid())
    {
        close(_socketFd);
        _socketFd = -1;
        _isNonBlocking = false;
    }
}
