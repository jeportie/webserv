/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/24 12:13:40 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "../../../include/webserv.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "SocketLogMessages.hpp"

// Constructor - Destructor

ClientSocket::ClientSocket()
: Socket()
, _clientAddrLen(sizeof(_clientAddr))
, _buffer()
, _isHeadersParsed(false)
, _bodyMode(BODY_NONE)
, _contentLength(0)
, _requestLine()
, _parsedHeaders()
, _isChunked(false)
, _chunkSize(0)
, _bodyAccumulator()
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_CLIENTSOCKET_CONSTRUCTOR, "ClientSocket::ClientSocket()");
    // On initialise _clientAddr après l’appel à Socket()
    std::memset(&_clientAddr, 0, sizeof(_clientAddr));
}

ClientSocket::~ClientSocket(void)
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_CLIENTSOCKET_DESTRUCTOR, "ClientSocket::~ClientSocket()");
    closeSocket();
}

// GETTERS

std::string& ClientSocket::getBuffer() { return (_buffer); }

int ClientSocket::getClientPort(void) const { return (_clientAddr.sin_port); }

socklen_t ClientSocket::getClientAddrLen(void) const { return (_clientAddrLen); }

const struct sockaddr_in& ClientSocket::getClientAddr(void) const { return (_clientAddr); }

time_t ClientSocket::getLastActivity() const { return _lastActivity; }

size_t ClientSocket::getContentLength() const { return (_contentLength); }

SVSMAP ClientSocket::getParsedHeaders() const { return (_parsedHeaders); }

bool ClientSocket::getIsHeadersParsed() const { return (_isHeadersParsed); }

RequestLine ClientSocket::getRequestLine() const { return (_requestLine); }

BodyMode ClientSocket::getBodyMode() const { return (_bodyMode); }

std::string& ClientSocket::getBodyAccumulator() { return _bodyAccumulator; }

bool ClientSocket::getIsChunked() const { return (_isChunked); }

size_t ClientSocket::getChunkSize() const { return (_chunkSize); }

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

void ClientSocket::setHeadersParsed(bool parsed) { _isHeadersParsed = parsed; }

void ClientSocket::setContentLength(size_t length) { _contentLength = length; }

void ClientSocket::setRequestLine(RequestLine rl) { _requestLine = rl; }

void ClientSocket::setBodyMode(BodyMode mode) { _bodyMode = mode; }

void ClientSocket::setChunked(bool c) { _isChunked = c; }

void ClientSocket::setChunkSize(size_t s) { _chunkSize = s; }

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

void ClientSocket::setParsedHeaders(SVSMAP hdrs)
{
    _parsedHeaders = hdrs;
}
