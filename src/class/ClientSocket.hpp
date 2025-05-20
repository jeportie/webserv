/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:15:21 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "Socket.hpp"
# include <netinet/in.h>
# include <sys/socket.h>
# include <string>

class ClientSocket : public Socket
{
public:

    ClientSocket(void);
    virtual ~ClientSocket(void);

    virtual int setNonBlocking(int fd);
	virtual int safeFcntl(int fd, int cmd, int flag);
    
    std::string getClientIP(void) const;
    int getClientPort(void) const;
    void setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen);
    const struct sockaddr_in& getClientAddr(void) const;
    socklen_t getClientAddrLen(void) const;

private:

    ClientSocket(const ClientSocket& src);
    ClientSocket& operator=(const ClientSocket& rhs);

    struct sockaddr_in _clientAddr;  ///< Client address structure
    socklen_t _clientAddrLen;        ///< Length of client address structure
};

#endif  // ************************************************* CLIENTSOCKET_HPP //

