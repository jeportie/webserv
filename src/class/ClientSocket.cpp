/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 17:18:38 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/webserv.h"
#include "ClientSocket.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

/**
 * @brief Default constructor
 *
 * Initializes the base Socket class and sets up the client address structure.
 * The address structure is zeroed out and the address length is set to the
 * size of the structure.
 */
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
    std::memset(&_clientAddr, 0, sizeof(_clientAddr));
}



/**
 * @brief Destructor
 *
 * Closes the socket if it's still open and releases any resources.
 * Calls the closeSocket method from the base Socket class.
 */
ClientSocket::~ClientSocket(void)
{
	closeSocket();
}

/**
 * @brief set socket flags
 *
 * @param fd The file descriptor
 * @param cmd option flag
 * @param the flag option
 *
 * @return int Return value from fcntl call, -1 on error
 */
int ClientSocket::safeFcntl(int fd, int cmd, int flag)
{
	int	ret;

	ret = fcntl(fd, cmd, flag);
	if (ret == -1)
	{
		std::cerr << "[Error] fcntl failed on client fd " << fd << ": " << strerror(errno) << std::endl;
		return (-1);
	}
	return (ret);
}

/**
 * @brief Sets a file descriptor to non-blocking mode
 *
 * Uses fcntl to get the current flags for the file descriptor,
 * adds the O_NONBLOCK flag, and sets the new flags.
 * Updates the _isNonBlocking flag if successful.
 *
 * @param fd The file descriptor to set to non-blocking mode
 * @return int Return value from fcntl call, -1 on error
 */
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

/**
 * @brief Gets the client's port number
 *
 * Converts the port number from network byte order to host byte order
 * using a custom function to convert the IP address from network byte
 * order to a string.
 *
 * @return int The client's port number
 */
int ClientSocket::getClientPort(void) const
{
	return (_clientAddr.sin_port);
}

/**
 * @brief Gets the client's IP address as a string
 *
 * Converts the stored network address to a human-readable string
 * using the inet_ntop (network to presentation) function.
 *
 * @return std::string The client's IP address
 */
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
	oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1]) << "." << static_cast<int>(bytes[2]) << "." << static_cast<int>(bytes[3]);

	return (oss.str());
}

/**
 * @brief Sets the client's address information
 *
 * Used when accepting a new client connection to store the client's
 * address information for later use.
 *
 * @param addr The client's address structure
 * @param addrLen The length of the address structure
 */
void ClientSocket::setClientAddr(const struct sockaddr_in &addr,
	socklen_t addrLen)
{
	_clientAddr = addr;
	_clientAddrLen = addrLen;
}

/**
 * @brief Gets the client's address structure
 *
 * @return const struct sockaddr_in& Reference to the client's address structure
 */
const struct sockaddr_in &ClientSocket::getClientAddr(void) const
{
	return (_clientAddr);
}

/**
 * @brief Gets the length of the client's address structure
 *
 * @return socklen_t The length of the address structure
 */
socklen_t ClientSocket::getClientAddrLen(void) const
{
	return (_clientAddrLen);
}

std::string &ClientSocket::getBuffer()
{
	return (_buffer);
}

bool ClientSocket::headersParsed() const
{
	return (_headersParsed);
}

void ClientSocket::setHeadersParsed(bool parsed)
{
	_headersParsed = parsed;
}

size_t ClientSocket::getContentLength() const
{
	return (_contentLength);
}

RequestLine ClientSocket::getRequestLine() const
{
	return (_requestLine);
}

std::map<std::string,
	std::vector<std::string> > ClientSocket::getParsedHeaders() const
{
	return (_parsedHeaders);
}

void ClientSocket::setContentLength(size_t length)
{
	_contentLength = length;
}

void ClientSocket::setRequestLine(RequestLine rl)
{
	_requestLine = rl;
}
void ClientSocket::setParsedHeaders(std::map<std::string,
	std::vector<std::string> > hdrs)
{
	_parsedHeaders = hdrs;
}

bool ClientSocket::getChunked() const
{
	return (_chunked);
}
void ClientSocket::setChunked(bool c)
{
	_chunked = c;
}
size_t ClientSocket::getChunkSize() const
{
	return (_chunkSize);
}
void ClientSocket::setChunkSize(size_t s)
{
	_chunkSize = s;
}
std::string& ClientSocket::getBodyAccumulator()
{
    return _bodyAccumulator;
}

void ClientSocket::clearBodyAccumulator()
{
    _bodyAccumulator.clear();
}

BodyMode ClientSocket::getBodyMode() const
{
	return (_bodyMode);
}
void    ClientSocket::setBodyMode(BodyMode mode)
{
	_bodyMode = mode;
}

void ClientSocket::determineBodyMode()
{
    // Initialisation par défaut : pas de corps
    _bodyMode = BODY_NONE;
    _chunked = false;
    _contentLength = 0;
	
    // Recherche "chunked" dans Transfer-Encoding
    if (_parsedHeaders.count("Transfer-Encoding")) {
        const std::vector<std::string>& list = _parsedHeaders["Transfer-Encoding"];
        for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it) {
            std::string lower = *it;
            // conversion en minuscules
            for (std::string::size_type i = 0; i < lower.size(); ++i)
                lower[i] = static_cast<char>(std::tolower(lower[i]));
            if (lower == "chunked") {
                _bodyMode = BODY_CHUNKED;
                _chunked = true;
                return;
            }
        }
    }
    // Sinon, Content-Length
    if (_parsedHeaders.count("Content-Length") && !_parsedHeaders["Content-Length"].empty()) {
        _bodyMode = BODY_CONTENT_LENGTH;
        _contentLength = std::atoi(_parsedHeaders["Content-Length"][0].c_str());
    }
}

void ClientSocket::resetParserState()
{
    // On ne ferme pas la socket ici, on ne s’occupe que du parsing
    _buffer.clear();
    _headersParsed = false;
    _bodyMode      = BODY_NONE;
    _contentLength = 0;
    _requestLine   = RequestLine();         // remet à défaut
    _parsedHeaders.clear();

    _chunked        = false;
    _chunkSize      = 0;
    _bodyAccumulator.clear();
}


// SocketManager.cpp.cpp
