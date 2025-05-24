/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.api.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/24 11:43:27 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "SocketLogMessages.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"
#include "../../../include/webserv.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

void   ClientSocket::touch()
{
	std::string clientID;

	clientID = _socketFd;
	LOG_SYSTEM_ERROR(DEBUG, SOCKET_ERROR, LOG_UPDATED_TIMESTAMP + clientID, __FUNCTION__);
	_lastActivity = time(NULL); 
}

int ClientSocket::safeFcntl(int fd, int cmd, int flag)
{
    int					ret;
    std::stringstream	ss;

	ret = fcntl(fd, cmd, flag);
    if (ret == -1)
    {
        ss << fd;
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, LOG_FCNTL_FAILED + ss.str(), __FUNCTION__);
        return (-1);
    }
    return (ret);
}

void ClientSocket::checkChunkedBodyMode()
{
    std::vector<std::string>::const_iterator it;
    std::string lower;
    std::string::size_type i;

    // Recherche "chunked" dans Transfer-Encoding
    if (_parsedHeaders.count("Transfer-Encoding"))
    {
        const std::vector<std::string>& list = _parsedHeaders["Transfer-Encoding"];
        for (it = list.begin(); it != list.end(); ++it)
        {
            lower = *it;
            // conversion en minuscules
            for (i = 0; i < lower.size(); ++i)
                lower[i] = static_cast<char>(std::tolower(lower[i]));
            if (lower == "chunked")
            {
                _bodyMode = BODY_CHUNKED;
                _isChunked  = true;
                return;
                if (lower != "chunked" && lower != "identity")
                    throw HttpException(501, LOG_NOT_IMPLEMENTED);
            }
        }
    }
}

void ClientSocket::checkContentLengthBodyMode()
{
    // Sinon, Content-Length
    if (_parsedHeaders.count("Content-Length") && !_parsedHeaders["Content-Length"].empty())
    {
        _bodyMode      = BODY_CONTENT_LENGTH;
        _contentLength = std::atoi(_parsedHeaders["Content-Length"][0].c_str());
        if (_contentLength > MAX_BODY_SIZE)
            throw HttpException(413, LOG_PAYLOAD_TOO_LARGE);
    }
}

void ClientSocket::determineBodyMode()
{
    std::vector<std::string>::const_iterator it;
    std::string lower;

    // Initialisation par défaut : pas de corps
    _bodyMode      = BODY_NONE;
    _isChunked     = false;
    _contentLength = 0;

    checkChunkedBodyMode();
    if (_bodyMode == BODY_CHUNKED)
        return;
    checkContentLengthBodyMode();
}

void ClientSocket::resetParserState()
{
    _buffer.clear();
    _isHeadersParsed = false;
    _bodyMode      = BODY_NONE;
    _contentLength = 0;
    _requestLine   = RequestLine();  // remet à défaut
    _parsedHeaders.clear();
    _isChunked   = false;
    _chunkSize = 0;
    _bodyAccumulator.clear();
}

void ClientSocket::clearBodyAccumulator() { _bodyAccumulator.clear(); }
