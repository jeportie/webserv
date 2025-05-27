/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestData.api.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:40:52 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 18:22:42 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestData.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"
#include "../../../include/webserv.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

void RequestData::checkChunkedBodyMode()
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

void RequestData::checkContentLengthBodyMode()
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

void RequestData::determineBodyMode()
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

void RequestData::resetParserState()
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

void RequestData::clearBodyAccumulator() { _bodyAccumulator.clear(); }

std::string RequestData::findHostInHeaders()
{
    std::string rtn;
    rtn = "";
    
    SVSMAP::iterator it;
    
    it = _parsedHeaders.begin();

    for (; it != _parsedHeaders.end() ; ++it)
    {
        if (it->first == "host")
            return (it->second[0]);
    }
    return (rtn);
    
}

int RequestData::getPortFromFd(int fd) {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    std::memset(&addr, 0, sizeof(addr));

    if (getsockname(fd, (struct sockaddr*)&addr, &addrLen) == -1) {
        perror("getsockname");
        return -1; // ou throw une exception selon ton design
    }

    return ntohs(addr.sin_port);  // convertit en port lisible (host byte order)
}

ServerConfig RequestData::findMyConfig(int port, std::string host, IVSCMAP ServerConfigMap)
{
    IVSCMAP::iterator itport;
    std::vector<ServerConfig>::iterator itServerConfig;
    std::vector<std::string>::iterator itServerNames;
    ServerConfig    myconfig;

    itport = ServerConfigMap.begin();
    for(; itport != ServerConfigMap.end(); ++itport) // je boucle sur les port
    {
        if (itport->first == port)
        {
            itServerConfig = itport->second.begin();
            if (host.empty())
                return (*itServerConfig);
            for (; itServerConfig!= itport->second.end(); ++itServerConfig) // le boucle sur le vecter ServerConfig
            {
                myconfig = *itServerConfig;
                itServerNames = myconfig.serverNames.begin();
                for (; itServerNames != myconfig.serverNames.end(); ++itServerNames) // je boucle sur le ServerNames du Serverconfig
                {
                    if (*itServerNames == host)
                    {
                        return (myconfig);
                    }
                }
            }
        }
    }
    throw HttpException(400, "Bad Request");
    // si je n'ai pas trouve de server name correspondant je send un 400 bad request.
}

void RequestData::initServerConfig(IVSCMAP ServerConfigMap)
{
    std::string host;
    int port;
    
    port = getPortFromFd(_listeningSocketFd);
    host = findHostInHeaders();
    
    _serverConfig = findMyConfig(port, host, ServerConfigMap);
    
}

