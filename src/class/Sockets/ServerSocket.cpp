/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:42:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 23:16:41 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Socket.hpp"
#include "ClientSocket.hpp"
#include <sys/epoll.h>
#include <iostream>
#include <sstream>
#include <cerrno>
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"

ServerSocket::ServerSocket(void)
: Socket()
{
    std::memset(&this->_serverAddr, 0, sizeof(sockaddr_in));
}

ServerSocket::~ServerSocket(void) { closeSocket(); }

int ServerSocket::safeFcntl(int fd, int cmd, int flag)
{
    int ret = fcntl(fd, cmd, flag);
    if (ret == -1)
        THROW_SYSTEM_ERROR(
            CRITICAL, SOCKET_ERROR, "fcntl server socket failed", "ServerSocket::safeFcntl");
    return ret;
}

int ServerSocket::setNonBlocking(int fd)
{
    int flags = safeFcntl(fd, F_GETFL, 0);
    int ret   = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);

    _isNonBlocking = true;

    return (ret);
}

bool ServerSocket::safeBind(int port, const std::string& adress)
{
    // Create socket if it doesn't exist
    if (!isValid() && !socketCreate())
    {
        LOG_ERROR(ERROR, SOCKET_ERROR, "Failed to create socket", "ServerSocket::safeBind");
        return false;
    }

    (void) adress;
    setNonBlocking(this->_socketFd);
    _serverAddr.sin_port   = htons(port);
    _serverAddr.sin_family = AF_INET;

    _serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, "Bind failed", "ServerSocket::safeBind");
        return false;
    }
    return (true);
}

void ServerSocket::safeListen(int backlog)
{
    if (!isValid())
    {
        THROW_ERROR(
            CRITICAL, SOCKET_ERROR, "Cannot listen on invalid socket", "ServerSocket::safeListen");
    }
    if (listen(this->_socketFd, backlog) < 0)
    {
        THROW_SYSTEM_ERROR(CRITICAL, SOCKET_ERROR, "Listen() Failed", "ServerSocket::safeListen");
    }
}

ClientSocket* ServerSocket::safeAccept(int epoll_fd)
{
    if (!isValid())
    {
        THROW_ERROR(
            CRITICAL, SOCKET_ERROR, "Cannot accept on invalid socket", "ServerSocket::safeAccept");
    }

    ClientSocket*      client = new ClientSocket();
    struct sockaddr_in clientAddr;
    socklen_t          clientAddrLen = sizeof(clientAddr);

    int clientFd = accept(this->_socketFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if (clientFd < 0)
    {
        delete client;
        THROW_SYSTEM_ERROR(CRITICAL, SOCKET_ERROR, "Accept() Failed", "ServerSocket::safeAccept");
    }

    client->setFd(clientFd);
    client->setClientAddr(clientAddr, clientAddrLen);
    client->setNonBlocking(clientFd);

    std::cout << "Connection accepted from " << client->getClientIP() << ":"
              << client->getClientPort() << std::endl;

    // If epoll_fd is valid, register the client with epoll
    if (epoll_fd >= 0)
    {
        epoll_event client_ev;
        client_ev.events  = EPOLLIN | EPOLLET;
        client_ev.data.fd = clientFd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &client_ev) < 0)
        {
            delete client;
            THROW_SYSTEM_ERROR(
                CRITICAL, EPOLL_ERROR, "Failed to add client to epoll", "ServerSocket::safeAccept");
        }
    }

    return (client);
}

int ServerSocket::getPort(void) const { return ntohs(_serverAddr.sin_port); }

std::string ServerSocket::getAddress(void) const
{
    // Convert network byte order to host byte order
    uint32_t ip = ntohl(_serverAddr.sin_addr.s_addr);

    // Extract each byte
    unsigned char bytes[4];
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    // Convert to string using C++ streams
    std::ostringstream oss;
    oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1]) << "."
        << static_cast<int>(bytes[2]) << "." << static_cast<int>(bytes[3]);

    return oss.str();
}
