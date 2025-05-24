/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/24 11:33:36 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#define LOG_CLIENTSOCKET_CONSTRUCTOR "ClientSocket Constructor called."
#define LOG_CLIENTSOCKET_DESTRUCTOR "ClientSocket Destructor called."

#define SVSMAP std::map<std::string, std::vector<std::string> > 

#include "Socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include "../Http/RequestLine.hpp"

enum BodyMode
{
    BODY_NONE,
    BODY_CONTENT_LENGTH,
    BODY_CHUNKED
};

class ClientSocket : public Socket
{
public:

    ClientSocket(void);
    virtual ~ClientSocket(void);

	void						touch(void);
    virtual int                 safeFcntl(int fd, int cmd, int flag);
    void                        determineBodyMode();
    void                        clearBodyAccumulator();
    void                        resetParserState();
	
	// Getters
    std::string&				getBuffer();
    std::string                 getClientIP(void) const;
    int							getClientPort(void) const;
    const struct sockaddr_in&	getClientAddr(void) const;
    socklen_t                   getClientAddrLen(void) const;
	time_t						getLastActivity(void) const;
    size_t                      getContentLength() const;
    SVSMAP						getParsedHeaders() const;
    bool                        getIsHeadersParsed() const;
    RequestLine                 getRequestLine() const;
    BodyMode                    getBodyMode() const;
    std::string&                getBodyAccumulator();
    bool                        getIsChunked() const;
    size_t                      getChunkSize() const;
	
	// Setters
    virtual int					setNonBlocking(int fd);
    void                        setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen);
    void                        setHeadersParsed(bool parsed);
    void                        setContentLength(size_t length);
    void						setRequestLine(RequestLine rl);
    void                        setBodyMode(BodyMode mode);
    void                        setParsedHeaders(SVSMAP hdrs);
    void                        setChunked(bool);
    void                        setChunkSize(size_t);

private:
    ClientSocket(const ClientSocket& src);
    ClientSocket& operator=(const ClientSocket& rhs);

    void checkChunkedBodyMode();
    void checkContentLengthBodyMode();
    
	time_t						_lastActivity;
    socklen_t                   _clientAddrLen;  ///< Length of client address structure
    struct sockaddr_in          _clientAddr;     ///< Client address structure
    std::string                 _buffer;
    bool                        _isHeadersParsed;
    BodyMode                    _bodyMode;
    size_t                      _contentLength;
    RequestLine                 _requestLine;
    SVSMAP						_parsedHeaders;
    bool                        _isChunked;    // mode chunked activé
    size_t                      _chunkSize;  // taille restante du chunk courant
    std::string                 _bodyAccumulator;
};

#endif  // ************************************************* CLIENTSOCKET_HPP //
