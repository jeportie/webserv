/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:32:17 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/03 17:46:46 by anastruc         ###   ########.fr       */
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
#include <iostream>

Socket::Socket(void)
: _socketFd(-1)
, _isNonBlocking(false)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SOCKET_CONSTRUCTOR, __FUNCTION__);
}

Socket::~Socket(void)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_SOCKET_DESTRUCTOR, __FUNCTION__);
    // std::cout << "JE CLOSE LA SOCKET" << std::endl;
    // closeSocket();
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
    {
        _isNonBlocking = rhs._isNonBlocking;
        _socketFd = rhs._socketFd;
    }
    return (*this);
}
