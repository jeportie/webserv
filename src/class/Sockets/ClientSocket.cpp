/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 17:48:56 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

// Constructor - Destructor

ClientSocket::ClientSocket()
: Socket()
, _clientAddrLen(sizeof(_clientAddr))

{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_CLIENTSOCKET_CONSTRUCTOR, __FUNCTION__);
    // On initialise _clientAddr après l’appel à Socket()
    std::memset(&_clientAddr, 0, sizeof(_clientAddr));
}

ClientSocket::~ClientSocket(void)
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_CLIENTSOCKET_DESTRUCTOR, __FUNCTION__);
    closeSocket();
}

// GETTERS


int ClientSocket::getClientPort(void) const { return (_clientAddr.sin_port); }

socklen_t ClientSocket::getClientAddrLen(void) const { return (_clientAddrLen); }

const struct sockaddr_in& ClientSocket::getClientAddr(void) const { return (_clientAddr); }

time_t ClientSocket::getLastActivity() const { return _lastActivity; }

std::string ClientSocket::getClientIP(void) const
{
    uint32_t			ip;
    unsigned char		bytes[4];
    std::ostringstream	oss;

    // Convert network byte order to host byte order
    ip = ntohl(_clientAddr.sin_addr.s_addr);
    // Extract each byte
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1]) << "."
        << static_cast<int>(bytes[2]) << "." << static_cast<int>(bytes[3]);

    return (oss.str());
}

// SETTERS


int ClientSocket::setNonBlocking(int fd)
{
    int flags;
    int ret;

    // Get current flags
    flags = safeFcntl(fd, F_GETFL, 0);
    // Set new flags with O_NONBLOCK added
    ret = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);
    // Update non-blocking flag
    _isNonBlocking = true;

    return (ret);
}

void ClientSocket::setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen)
{
    _clientAddr    = addr;
    _clientAddrLen = addrLen;
}
