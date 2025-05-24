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
#define SOCKET_HPP

#define LOG_SOCKET_CREATE_FAIL "Failed to create socket"
#define LOG_FCNTL_SERVER_FAIL "fcntl server socket failed"
#define LOG_BIND_FAIL "Bind failed"
#define LOG_LISTEN_FAIL "Listen() Failed"
#define LOG_INVALID_SOCKET_LISTEN "Cannot listen on invalid socket"
#define LOG_INVALID_SOCKET_ACCEPT "Cannot accept on invalid socket"
#define LOG_ACCEPT_FAIL "Accept() Failed"
#define LOG_NO_MORE_CLIENTS "No more Clients to Accept in the queue"
#define LOG_ACCEPTED_CONNECTION "Connection accepted from "
#define LOG_ADD_CLIENT_EPOLL_FAIL "Failed to add client to epoll"
#define LOG_UPDATED_TIMESTAMP "Updated Timestamp of client fd="
#define LOG_FCNTL_FAILED "fcntl failed on client fd "
#define LOG_NOT_IMPLEMENTED "Not Implemented"
#define LOG_PAYLOAD_TOO_LARGE "Payload Too Large"
#define LOG_SOCKET_CONSTRUCTOR "Socket Constructor called"
#define LOG_SOCKET_DESTRUCTOR "Socket Desstructor called"
#define LOG_SOCKET_COPY_CONSTRUCTOR "Socket Copy constructor called"
#define LOG_ERROR_CREATING_SOCKET "Error creating socket: "
#define LOG_CANNOT_SET_SO_REUSEADDR "Cannot set SO_REUSEADDR on invalid socket"
#define LOG_ERROR_SETTING_SO_REUSEADDR "Error setting SO_REUSEADDR: "

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

class Socket
{
public:
    Socket(void);
    virtual ~Socket(void);

    bool socketCreate(void);

    virtual int setNonBlocking(int fd)               = 0;
    virtual int safeFcntl(int fd, int cmd, int flag) = 0;

    bool setReuseAddr(bool reuse = true);
    int  getFd(void) const;
    void setFd(int fd);
    bool isValid(void) const;
    bool isNonBlocking(void) const;
    void closeSocket(void);

protected:
    Socket(const Socket& src);
    Socket& operator=(const Socket& rhs);

    int  _socketFd;
    bool _isNonBlocking;
};

#endif  // ******************************************************* SOCKET_HPP //
