/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:32:17 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 18:11:29 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

#include <cmath>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>

Socket::Socket(void)
: _socketFd(-1)
, _isNonBlocking(false)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SOCKET_CONSTRUCTOR, __FUNCTION__);
}

Socket::~Socket(void)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SOCKET_DESTRUCTOR, __FUNCTION__);
    closeSocket();
}

Socket::Socket(const Socket& src)
: _socketFd(-1)
, _isNonBlocking(false)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SOCKET_COPY_CONSTRUCTOR, __FUNCTION__);
    *this = src;
}

Socket& Socket::operator=(const Socket& rhs)
{
    if (this != &rhs)
        _isNonBlocking = rhs._isNonBlocking;
    return (*this);
}
