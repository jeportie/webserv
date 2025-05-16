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
# define CLIENTSOCKET_HPP

# include "Socket.hpp"
# include <netinet/in.h>
# include <sys/socket.h>
# include <string>
#include <vector>
#include "RequestLine.hpp"

enum BodyMode {
    BODY_NONE,
    BODY_CONTENT_LENGTH,
    BODY_CHUNKED
};

/**
 * @brief Client socket class that represents a connected client
 * 
 * This class extends the Socket base class to provide functionality
 * specific to client connections. It stores client address information
 * and provides methods to access this information.
 */
class ClientSocket : public Socket
{
public:
    /**
     * @brief Default constructor
     */
    ClientSocket(void);
    virtual ~ClientSocket(void);

    /**
     * Implementations of pure virtual method from Socket base class
     */
    virtual int setNonBlocking(int fd);
	virtual int safeFcntl(int fd, int cmd, int flag);
    
    /**
     * @brief Gets the client's IP address as a string
     * Converts the stored network address to a human-readable string
     * 
     * @return std::string The client's IP address
     */
    std::string getClientIP(void) const;
    
    /**
     * @brief Gets the client's port number
     * Converts from network byte order to host byte order
     * 
     * @return int The client's port number
     */
    int getClientPort(void) const;
    
    /**
     * @brief Sets the client's address information
     * Used when accepting a new client connection
     * 
     * @param addr The client's address structure
     * @param addrLen The length of the address structure
     */
    void setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen);
    
    /**
     * @brief Gets the client's address structure
     * 
     * @return const struct sockaddr_in& Reference to the client's address structure
     */
    const struct sockaddr_in& getClientAddr(void) const;
    
    /**
     * @brief Gets the length of the client's address structure
     * 
     * @return socklen_t The length of the address structure
     */
    socklen_t getClientAddrLen(void) const;

    // --- Contexte de parsing HTTP ---
    /// Accès au tampon brut où l’on accumulate les données lues
    std::string&       getBuffer();
    /// Indicateur : avons-nous déjà parsé les headers ?
    bool               headersParsed() const;
    void               setHeadersParsed(bool parsed);
    /// Valeur Content-Length (ou 0 si chunked ou non encore extrait)
    size_t             getContentLength() const;
    std::map<std::string,std::vector<std::string> > getParsedHeaders() const;
    RequestLine        getRequestLine() const;
    void               setContentLength(size_t length);
    void               setRequestLine(RequestLine rl);
    BodyMode            getBodyMode() const;
    void                setBodyMode(BodyMode mode);
    void                determineBodyMode();



    std::string&       getBodyAccumulator();
    void               clearBodyAccumulator();
    void               resetParserState();
    void               setParsedHeaders(std::map<std::string,std::vector<std::string> > hdrs);
    bool               getChunked() const;
    void               setChunked(bool);
    size_t             getChunkSize() const;
    void               setChunkSize(size_t);
    

private:

    socklen_t _clientAddrLen;        ///< Length of client address structure
    struct sockaddr_in _clientAddr;  ///< Client address structure
    std::string                                     _buffer;
    bool                                            _headersParsed;
    BodyMode                                        _bodyMode;
    size_t                                          _contentLength;
    RequestLine                                     _requestLine;
    std::map<std::string,std::vector<std::string> >  _parsedHeaders;
    bool                                            _chunked;         // mode chunked activé
    size_t                                          _chunkSize;       // taille restante du chunk courant
    std::string                                     _bodyAccumulator;

    /**
     * @brief Copy constructor (private to prevent copying)
     * 
     * @param src The source object to copy from
     */
    ClientSocket(const ClientSocket& src);
    
    /**
     * @brief Assignment operator (private to prevent assignment)
     * 
     * @param rhs The right-hand side object to assign from
     * @return ClientSocket& Reference to this object
     */
    ClientSocket& operator=(const ClientSocket& rhs);

};

#endif  // ************************************************* CLIENTSOCKET_HPP //

