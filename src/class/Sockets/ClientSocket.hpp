/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/27 16:51:53 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#define LOG_CLIENTSOCKET_CONSTRUCTOR "ClientSocket Constructor called."
#define LOG_CLIENTSOCKET_DESTRUCTOR "ClientSocket Destructor called."

#include "Socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include "../Http/RequestLine.hpp"
#include "../../../include/webserv.h"
#include "../ConfigFile/ServerConfig.hpp"
#include "RequestData.hpp"


class ClientSocket : public Socket
{
public:

    ClientSocket(void);
    virtual ~ClientSocket(void);

	void						touch(void);
    virtual int                 safeFcntl(int fd, int cmd, int flag);

	// Getters
    const struct sockaddr_in&	getClientAddr(void) const;
    socklen_t                   getClientAddrLen(void) const;
	time_t						getLastActivity(void) const;
    std::string                 getClientIP(void) const;
    int							getClientPort(void) const;
	// Setters
    virtual int					setNonBlocking(int fd);
    void                        setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen);
    
    RequestData                 requestData;

private :

    ClientSocket(const ClientSocket& src);
    ClientSocket& operator=(const ClientSocket& rhs);

    
    // requetedata
    socklen_t                   _clientAddrLen;  ///< Length of client address structure
    struct sockaddr_in          _clientAddr;     ///< Client address structure
    time_t						_lastActivity;


};

typedef std::map<int, ClientSocket*>						ICMAP;

#endif  // ************************************************* CLIENTSOCKET_HPP //
