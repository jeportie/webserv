/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestData.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:22:15 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:04:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTDATA_HPP
#define REQUESTDATA_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#include "../../../include/webserv.hpp"
#include "../ConfigFile/ServerConfig.hpp"
#include "RequestLine.hpp"

#define LOG_REQUESTDATA_CONSTRUCTOR "RequestData Constructor called."
#define LOG_REQUESTDATA_DESTRUCTOR "RequestData Destructor called."

enum BodyMode
{
    BODY_NONE,
    BODY_CONTENT_LENGTH,
    BODY_CHUNKED
};

// ATTENTION VIRTUAL ET DEUXIEME FONCTION INITSERVERCONFIG A ENLEVER UNIQUEMENT LA POUR LES TESTS
// UNITAIRE

class RequestData
{
public:
    RequestData(void);
    ~RequestData(void);

    void                determineBodyMode();
    void                clearBodyAccumulator();
    void                resetParserState();
    virtual std::string findHostInHeaders();
    virtual int         getPortFromFd(int fd);
    void                initServerConfig(IVSCMAP ServerConfigMap, int port, const std::string& host)
    {
        _serverConfig = findMyConfig(port, host, ServerConfigMap);
    }
    ServerConfig findMyConfig(int port, std::string host, IVSCMAP ServerConfigMap);
    void         initServerConfig(IVSCMAP ServerConfigMap);

    // Getters
    std::string& getBuffer();
    size_t       getContentLength() const;
    SVSMAP       getParsedHeaders() const;
    bool         getIsHeadersParsed() const;
    RequestLine  getRequestLine() const;
    BodyMode     getBodyMode() const;
    std::string& getBodyAccumulator();
    bool         getIsChunked() const;
    size_t       getChunkSize() const;

    // Setters
    void setHeadersParsed(bool parsed);
    void setContentLength(size_t length);
    void setRequestLine(RequestLine rl);
    void setBodyMode(BodyMode mode);
    void setParsedHeaders(SVSMAP hdrs);
    void setChunked(bool);
    void setChunkSize(size_t);

    void setListeningSocketFd(int fd) { _listeningSocketFd = fd; }
    int  getListeningSocketFd() const { return _listeningSocketFd; }

    ServerConfig getServerConfig() const { return _serverConfig; }

private:
    RequestData(const RequestData& src);
    RequestData& operator=(const RequestData& rhs);

    void checkChunkedBodyMode();
    void checkContentLengthBodyMode();

    // requetedata
    std::string  _buffer;
    bool         _isHeadersParsed;
    BodyMode     _bodyMode;
    size_t       _contentLength;
    RequestLine  _requestLine;
    SVSMAP       _parsedHeaders;
    bool         _isChunked;  // mode chunked activé
    size_t       _chunkSize;  // taille restante du chunk courant
    std::string  _bodyAccumulator;
    int          _listeningSocketFd;
    ServerConfig _serverConfig;

protected:
};

#endif  // ************************************************* CLIENTSOCKET_HPP //
