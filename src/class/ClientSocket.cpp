/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 22:48:22 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/webserv.h"
#include "ClientSocket.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <cerrno>
#include "ErrorHandler.hpp"

ClientSocket::ClientSocket(void)
: Socket()
{
    std::memset(&_clientAddr, 0, sizeof(sockaddr_in));
    _clientAddrLen = sizeof(_clientAddr);
}

ClientSocket::~ClientSocket(void) { closeSocket(); }

int ClientSocket::safeFcntl(int fd, int cmd, int flag)
{
    int ret = fcntl(fd, cmd, flag);
    if (ret == -1)
    {
        std::stringstream ss;
        ss << fd;
        LOG_SYSTEM_ERROR(ERROR,
                         SOCKET_ERROR,
                         "fcntl failed on client fd " + ss.str(),
                         "ClientSocket::safeFcntl");
        return -1;
    }
    return ret;
}

int ClientSocket::setNonBlocking(int fd)
{
    // Get current flags
    int flags = safeFcntl(fd, F_GETFL, 0);

    // Set new flags with O_NONBLOCK added
    int ret = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);

    // Update non-blocking flag
    _isNonBlocking = true;

    return (ret);
}

int ClientSocket::getClientPort(void) const { return (_clientAddr.sin_port); }

std::string ClientSocket::getClientIP(void) const
{
    // Convert network byte order to host byte order
    uint32_t ip = ntohl(_clientAddr.sin_addr.s_addr);

    // Extract each byte
    unsigned char bytes[4];
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    // Convert to string using C++ streams
    std::ostringstream oss;
    oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1]) << "."
        << static_cast<int>(bytes[2]) << "." << static_cast<int>(bytes[3]);

    return oss.str();
}

void ClientSocket::setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen)
{
    _clientAddr    = addr;
    _clientAddrLen = addrLen;
}

const struct sockaddr_in& ClientSocket::getClientAddr(void) const { return _clientAddr; }

socklen_t ClientSocket::getClientAddrLen(void) const { return _clientAddrLen; }
