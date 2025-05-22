/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/19 15:22:27 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../include/webserv.h"
#include "ClientSocket.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>


ClientSocket::ClientSocket()
: Socket()
, _clientAddrLen(sizeof(_clientAddr))
, _buffer()
, _headersParsed(false)
, _bodyMode(BODY_NONE)
, _contentLength(0)
, _requestLine()
, _parsedHeaders()
, _chunked(false)
, _chunkSize(0)
, _bodyAccumulator()
{
    // On initialise _clientAddr après l’appel à Socket()
	//
    std::memset(&_clientAddr, 0, sizeof(_clientAddr));
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

    return (oss.str());
}


void ClientSocket::setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen)
{
    _clientAddr    = addr;
    _clientAddrLen = addrLen;
}


const struct sockaddr_in& ClientSocket::getClientAddr(void) const { return (_clientAddr); }

socklen_t ClientSocket::getClientAddrLen(void) const { return (_clientAddrLen); }

std::string& ClientSocket::getBuffer() { return (_buffer); }

bool ClientSocket::headersParsed() const { return (_headersParsed); }

void ClientSocket::setHeadersParsed(bool parsed) { _headersParsed = parsed; }

size_t ClientSocket::getContentLength() const { return (_contentLength); }

RequestLine ClientSocket::getRequestLine() const { return (_requestLine); }

std::map<std::string, std::vector<std::string> > ClientSocket::getParsedHeaders() const
{
    return (_parsedHeaders);
}

void ClientSocket::setContentLength(size_t length) { _contentLength = length; }

void ClientSocket::setRequestLine(RequestLine rl) { _requestLine = rl; }
void ClientSocket::setParsedHeaders(std::map<std::string, std::vector<std::string> > hdrs)
{
    _parsedHeaders = hdrs;
}

void ClientSocket::touch() { _lastActivity = time(NULL); }
time_t ClientSocket::getLastActivity() const { return _lastActivity; }

bool         ClientSocket::getChunked() const { return (_chunked); }
void         ClientSocket::setChunked(bool c) { _chunked = c; }
size_t       ClientSocket::getChunkSize() const { return (_chunkSize); }
void         ClientSocket::setChunkSize(size_t s) { _chunkSize = s; }
std::string& ClientSocket::getBodyAccumulator() { return _bodyAccumulator; }

void ClientSocket::clearBodyAccumulator() { _bodyAccumulator.clear(); }

BodyMode ClientSocket::getBodyMode() const { return (_bodyMode); }
void     ClientSocket::setBodyMode(BodyMode mode) { _bodyMode = mode; }

void ClientSocket::determineBodyMode()
{
    // Initialisation par défaut : pas de corps
    _bodyMode      = BODY_NONE;
    _chunked       = false;
    _contentLength = 0;

    // Recherche "chunked" dans Transfer-Encoding
    if (_parsedHeaders.count("Transfer-Encoding"))
    {
        const std::vector<std::string>& list = _parsedHeaders["Transfer-Encoding"];
        for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it)
        {
            std::string lower = *it;
            // conversion en minuscules
            for (std::string::size_type i = 0; i < lower.size(); ++i)
                lower[i] = static_cast<char>(std::tolower(lower[i]));
            if (lower == "chunked")
            {
                _bodyMode = BODY_CHUNKED;
                _chunked  = true;
                return;
                if (lower != "chunked" && lower != "identity")
                    throw HttpException(501, "Not Implemented");
            }
        }
    }
    // Sinon, Content-Length
    if (_parsedHeaders.count("Content-Length") && !_parsedHeaders["Content-Length"].empty())
    {
        _bodyMode      = BODY_CONTENT_LENGTH;
        _contentLength = std::atoi(_parsedHeaders["Content-Length"][0].c_str());
        if (_contentLength > MAX_BODY_SIZE)
            throw HttpException(413, "Payload Too Large");
    }
}

void ClientSocket::resetParserState()
{
    // On ne ferme pas la socket ici, on ne s’occupe que du parsing
    _buffer.clear();
    _headersParsed = false;
    _bodyMode      = BODY_NONE;
    _contentLength = 0;
    _requestLine   = RequestLine();  // remet à défaut
    _parsedHeaders.clear();

    _chunked   = false;
    _chunkSize = 0;
    _bodyAccumulator.clear();
}

int ClientSocket::setNonBlocking(int fd)
{
	int	flags;
	int	ret;

	// Get current flags
	flags = safeFcntl(fd, F_GETFL, 0);
	// Set new flags with O_NONBLOCK added
	ret = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);
	// Update non-blocking flag
	_isNonBlocking = true;
	return (ret);
}
// SocketManager.cpp.cpp
