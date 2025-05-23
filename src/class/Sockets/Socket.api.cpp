/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.api.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:32:17 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/23 16:34:13 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "../../../include/webserv.h"

#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <sys/socket.h>
#include <cerrno>

bool Socket::socketCreate(void)
{
	std::string msg;

    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
    {
		msg = strerror(errno);
		LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR,  "Error creating socket: " + msg,
			"Socket::socketCreate");
        return (false);
    }
    return (true);
}

bool Socket::setReuseAddr(bool reuse)
{
	int			option;
	std::string msg;

    if (!isValid())
    {
		LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, "Cannot set SO_REUSEADDR on invalid socket",
				   "Socket::setReuseAddr");
        return (false);
    }

    option = reuse ? 1 : 0;

    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
    {
		msg = strerror(errno);
		LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR,  "Error setting SO_REUSEADDR: " + msg,
			"Socket::setReuseAddr");
        return (false);
    }
    return (true);
}

void Socket::closeSocket(void)
{
    if (isValid())
    {
        close(_socketFd);
        _socketFd      = -1;
        _isNonBlocking = false;
    }
}

int Socket::getFd(void) const { return (_socketFd); }

void Socket::setFd(int fd) { _socketFd = fd; }

bool Socket::isValid(void) const { return (_socketFd != -1); }

bool Socket::isNonBlocking(void) const { return (_isNonBlocking); }
