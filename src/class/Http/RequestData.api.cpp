/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestData.api.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:40:52 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/04 10:19:57 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestData.hpp"
#include "HttpException.hpp"
#include "HttpLimits.hpp"
#include "../Sockets/Socket.hpp"
#include <iostream>
#include <stdio.h>

void RequestData::checkChunkedBodyMode()
{
    std::vector<std::string>::const_iterator it;
    std::string                              lower;
    std::string::size_type                   i;

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
                _bodyMode  = BODY_CHUNKED;
                _isChunked = true;
                return;
                if (lower != "chunked" && lower != "identity")
                    throw HttpException(501, LOG_NOT_IMPLEMENTED, "");
            }
        }
    }
}

void RequestData::checkContentLengthBodyMode()
{
    // Sinon, Content-Length
    if (_parsedHeaders.count("Content-Length") && !_parsedHeaders["Content-Length"].empty())
    {
        _bodyMode      = BODY_CONTENT_LENGTH;
        _contentLength = std::atoi(_parsedHeaders["Content-Length"][0].c_str());
        if (_contentLength > MAX_BODY_SIZE)
            throw HttpException(413, LOG_PAYLOAD_TOO_LARGE, "");
    }
}

void RequestData::determineBodyMode()
{
    std::vector<std::string>::const_iterator it;
    std::string                              lower;

    // Initialisation par défaut : pas de corps
    _bodyMode      = BODY_NONE;
    _isChunked     = false;
    _contentLength = 0;

    checkChunkedBodyMode();
    if (_bodyMode == BODY_CHUNKED)
        return;
    checkContentLengthBodyMode();
}

void RequestData::resetParserState()
{
    _buffer.clear();
    _isHeadersParsed = false;
    _bodyMode        = BODY_NONE;
    _contentLength   = 0;
    _requestLine     = RequestLine();  // remet à défaut
    _parsedHeaders.clear();
    _isChunked = false;
    _chunkSize = 0;
    _bodyAccumulator.clear();
}

void RequestData::clearBodyAccumulator() { _bodyAccumulator.clear(); }

std::string RequestData::findHostInHeaders()
{
    std::string rtn;
    rtn = "";

    SVSMAP::iterator it;

    it = _parsedHeaders.begin();

    for (; it != _parsedHeaders.end(); ++it)
    {
        if (it->first == "Host")
        {
            std::string host = it->second[0];
            size_t      pos  = host.find(':');
            if (pos != std::string::npos)
                return host.substr(0, pos);
            else
                return host;
        }
    }
    return (rtn);
}

int RequestData::getPortFromFd(int fd)
{
    struct sockaddr_in addr;
    socklen_t          addrLen = sizeof(addr);
    std::memset(&addr, 0, sizeof(addr));

    if (getsockname(fd, (struct sockaddr*) &addr, &addrLen) == -1)
    {
        throw HttpException(500, "Internal Server Error", "");
    }

    return ntohs(addr.sin_port);  // convertit en port lisible (host byte order)
}

ServerConfig RequestData::findMyConfig(int port, std::string host, IVSCMAP serverConfigMap)
{
    IVSCMAP::iterator itport = serverConfigMap.find(port);
    if (itport == serverConfigMap.end())
        throw HttpException(400, "Bad Request: port not found", "");

    std::vector<ServerConfig> configs = itport->second;

    if (configs.empty())
        throw HttpException(400, "Bad Request: no server config for port", "");

    if (host.empty())
        return configs[0];  // retourner la première config par défaut

    // Recherche manuelle car pas de lambda en C++98
    for (std::vector<ServerConfig>::iterator itConfig = configs.begin(); itConfig != configs.end();
         ++itConfig)
    {
        ServerConfig& config = *itConfig;

        for (std::vector<std::string>::iterator itServerName = config.serverNames.begin();
             itServerName != config.serverNames.end();
             ++itServerName)
        {
            if (*itServerName == host)
                return config;  // config trouvée
        }
    }

    throw HttpException(400, "Bad Request: no matching server name", "");
}

void RequestData::initServerConfig(IVSCMAP ServerConfigMap)
{
    std::string host;
    int         port;

    port = getPortFromFd(_listeningSocketFd);
    host = findHostInHeaders();

    _serverConfig = findMyConfig(port, host, ServerConfigMap);
}
