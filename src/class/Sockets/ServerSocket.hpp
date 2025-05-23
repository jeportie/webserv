/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:39 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/23 16:40:09 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"
#include <netinet/in.h>
#include <string>
#include <cstring>

class ServerSocket : public Socket
{
public:
    ServerSocket(void);
    virtual ~ServerSocket(void);

	virtual int			setNonBlocking(int fd);
    virtual int			safeFcntl(int fd, int cmd, int flag);

    bool				safeBind(int port, const std::string& address);
    void				safeListen(int backlog);
    ClientSocket*		safeAccept(int epoll_fd);

    int					getPort(void) const;
    std::string			getAddress(void) const;

private:
    ServerSocket(const ServerSocket& src);
    ServerSocket&		operator=(const ServerSocket& rhs);

    struct sockaddr_in	_serverAddr;  ///< Server address structure
};

#endif  // ************************************************* SERVERSOCKET_HPP //
