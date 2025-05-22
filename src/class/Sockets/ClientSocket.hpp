/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/16 16:13:59 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "Socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <vector>
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



    virtual int                                     setNonBlocking(int fd);
    virtual int                                     safeFcntl(int fd, int cmd, int flag);
    std::string                                     getClientIP(void) const;
    int                                             getClientPort(void) const;
    void                                            setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen);
    const struct sockaddr_in&                       getClientAddr(void) const;
    socklen_t                                       getClientAddrLen(void) const;

	void											touch(void);
	time_t											getLastActivity(void) const ;

    // --- Contexte de parsing HTTP ---
    /// Accès au tampon brut où l’on accumulate les données lues
    std::string& getBuffer();
    /// Indicateur : avons-nous déjà parsé les headers ?
    bool                                            headersParsed() const;
    void                                            setHeadersParsed(bool parsed);
    size_t                                          getContentLength() const;
    std::map<std::string, std::vector<std::string> > getParsedHeaders() const;
    RequestLine                                     getRequestLine() const;
    void                                            setContentLength(size_t length);
    void                                            setRequestLine(RequestLine rl);
    BodyMode                                        getBodyMode() const;
    void                                            setBodyMode(BodyMode mode);
    void                                            determineBodyMode();
    std::string&                                    getBodyAccumulator();
    void                                            clearBodyAccumulator();
    void                                            resetParserState();
    void                                            setParsedHeaders(std::map<std::string, std::vector<std::string> > hdrs);
    bool                                            getChunked() const;
    void                                            setChunked(bool);
    size_t                                          getChunkSize() const;
    void                                            setChunkSize(size_t);


private:
    ClientSocket(const ClientSocket& src);
    ClientSocket& operator=(const ClientSocket& rhs);
    
	time_t											_lastActivity;
    socklen_t                                       _clientAddrLen;  ///< Length of client address structure
    struct sockaddr_in                              _clientAddr;     ///< Client address structure
    std::string                                     _buffer;
    bool                                            _headersParsed;
    BodyMode                                        _bodyMode;
    size_t                                          _contentLength;
    RequestLine                                     _requestLine;
    std::map<std::string, std::vector<std::string> > _parsedHeaders;
    bool                                            _chunked;    // mode chunked activé
    size_t                                          _chunkSize;  // taille restante du chunk courant
    std::string                                     _bodyAccumulator;
};

#endif  // ************************************************* CLIENTSOCKET_HPP //
