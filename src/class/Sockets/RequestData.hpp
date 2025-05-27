/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestData.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:22:15 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 17:12:22 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTDATA_HPP
# define REQUESTDATA_HPP



#include "Socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include "../Http/RequestLine.hpp"
#include "../../../include/webserv.h"
#include "../ConfigFile/ServerConfig.hpp"
#include "RequestData.hpp"

#define LOG_REQUESTDATA_CONSTRUCTOR "RequestData Constructor called."
#define LOG_REQUESTDATA_DESTRUCTOR "RequestData Destructor called."



enum BodyMode
{
    BODY_NONE,
    BODY_CONTENT_LENGTH,
    BODY_CHUNKED
};


class RequestData
{
public:

    RequestData(void);
    ~RequestData(void);

    void                        determineBodyMode();
    void                        clearBodyAccumulator();
    void                        resetParserState();
	void                        initServerConfig();
    std::string                 findHostInHeaders()

	// Getters
    std::string&				getBuffer();
    size_t                      getContentLength() const;
    SVSMAP						getParsedHeaders() const;
    bool                        getIsHeadersParsed() const;
    RequestLine                 getRequestLine() const;
    BodyMode                    getBodyMode() const;
    std::string&                getBodyAccumulator();
    bool                        getIsChunked() const;
    size_t                      getChunkSize() const;
	
	// Setters
    void                        setHeadersParsed(bool parsed);
    void                        setContentLength(size_t length);
    void						setRequestLine(RequestLine rl);
    void                        setBodyMode(BodyMode mode);
    void                        setParsedHeaders(SVSMAP hdrs);
    void                        setChunked(bool);
    void                        setChunkSize(size_t);

private:
    RequestData(const RequestData& src);
    RequestData& operator=(const RequestData& rhs);

    void checkChunkedBodyMode();
    void checkContentLengthBodyMode();
    
    // requetedata
    std::string                 _buffer;
    bool                        _isHeadersParsed;
    BodyMode                    _bodyMode;
    size_t                      _contentLength;
    RequestLine                 _requestLine;
    SVSMAP						_parsedHeaders;
    bool                        _isChunked;    // mode chunked activé
    size_t                      _chunkSize;  // taille restante du chunk courant
    std::string                 _bodyAccumulator;
    int                         _listeningSocket;
    ServerConfig                _serverConfig;

    
};


#endif  // ************************************************* CLIENTSOCKET_HPP //
