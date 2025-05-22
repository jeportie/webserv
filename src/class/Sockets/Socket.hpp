/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:12:59 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:15:08 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstring>

class Socket
{
public:
    Socket(void);
    virtual ~Socket(void);

    bool socketCreate(void);
    
    virtual int setNonBlocking(int fd) = 0;
	virtual int safeFcntl(int fd, int cmd, int flag) = 0;
    
    bool setReuseAddr(bool reuse = true);
    int getFd(void) const;
    void setFd(int fd);
    bool isValid(void) const;
    bool isNonBlocking(void) const;
    void closeSocket(void);

protected:
    Socket(const Socket& src);
    Socket& operator=(const Socket& rhs);

    int _socketFd;       ///< Socket file descriptor
    bool _isNonBlocking; ///< Flag indicating if socket is in non-blocking mode
};

#endif  // ******************************************************* SOCKET_HPP //
