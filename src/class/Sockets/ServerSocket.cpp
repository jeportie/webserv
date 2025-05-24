/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:42:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/23 16:41:29 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Socket.hpp"
#include <sys/epoll.h>
#include <cerrno>
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"

ServerSocket::ServerSocket(void)
: Socket()
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "ServerSocket Constructor called.", "ServerSocket::ServerSocket()");
    std::memset(&this->_serverAddr, 0, sizeof(sockaddr_in));
}

ServerSocket::~ServerSocket(void)
{ 
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "ServerSocket Destructor called.", "ServerSocket::~ServerSocket()");
	closeSocket();
}
