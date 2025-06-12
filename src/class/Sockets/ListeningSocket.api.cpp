/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.api.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:42:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/05 10:40:48 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include <iostream>
#include <sstream>
#include <cerrno>

#include "ListeningSocket.hpp"
#include "ClientSocket.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

int ListeningSocket::safeFcntl(int fd, int cmd, int flag)
{
    int retFd;

    retFd = fcntl(fd, cmd, flag);
    if (retFd == -1)
        THROW_SYSTEM_ERROR(CRITICAL, SOCKET_ERROR, LOG_FCNTL_LISTENING_FAIL, __FUNCTION__);

    return (retFd);
}

int ListeningSocket::setNonBlocking(int fd)
{
    int retFd;

    retFd = safeFcntl(fd, F_SETFL, O_NONBLOCK);

    _isNonBlocking = true;

    return (retFd);
}

bool ListeningSocket::safeBind(int port, const std::string& adress)
{
    // Create socket if it doesn't exist
    if (!isValid() && !socketCreate())
    {
        LOG_ERROR(ERROR, SOCKET_ERROR, LOG_SOCKET_CREATE_FAIL, __FUNCTION__);
        return (false);
    }

    (void) adress;  // DO SOMETHING WIHT ADDR

    setNonBlocking(this->_socketFd);
    _ListeningAddr.sin_port        = htons(port);
    _ListeningAddr.sin_family      = AF_INET;
    _ListeningAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr*) &_ListeningAddr, sizeof(_ListeningAddr)) < 0)
    {
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, LOG_BIND_FAIL, __FUNCTION__);
        return (false);
    }
    return (true);
}

void ListeningSocket::safeListen(int backlog)
{
    if (!isValid())
    {
                THROW_ERROR(CRITICAL, SOCKET_ERROR, LOG_INVALID_SOCKET_LISTEN, __FUNCTION__);
    }

    if (listen(this->_socketFd, backlog) < 0)
    {
        THROW_SYSTEM_ERROR(CRITICAL, SOCKET_ERROR, LOG_LISTEN_FAIL, __FUNCTION__);
    }
}

ClientSocket* ListeningSocket::safeAccept(int epoll_fd)
{
    struct sockaddr_in clientAddr;
    epoll_event        client_ev;
    ClientSocket*      client;
    int                clientFd;
    socklen_t          clientAddrLen = sizeof(clientAddr);
    std::ostringstream oss;

    if (!isValid())
    {
        THROW_ERROR(CRITICAL, SOCKET_ERROR, LOG_INVALID_SOCKET_ACCEPT, __FUNCTION__);
    }

    client   = new ClientSocket();
    clientFd = accept(this->_socketFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if (clientFd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // No more clients to accept, not a real error
            delete client;
            LOG_ERROR(INFO, SOCKET_ERROR, LOG_NO_MORE_CLIENTS, __FUNCTION__);
            return (NULL);
        }
        delete client;
        THROW_SYSTEM_ERROR(CRITICAL, SOCKET_ERROR, LOG_ACCEPT_FAIL, __FUNCTION__);
    }
    client->requestData.setListeningSocketFd(this->_socketFd);
    client->setFd(clientFd);
    client->setClientAddr(clientAddr, clientAddrLen);
    client->setNonBlocking(clientFd);

    oss << LOG_ACCEPTED_CONNECTION << client->getClientIP() << ":" << client->getClientPort()
        << std::endl;
    // std::cout << oss.str();
    LOG_ERROR(INFO, SOCKET_ERROR, oss.str(), __FUNCTION__);

    // If epoll_fd is valid, register the client with epoll
    if (epoll_fd >= 0)
    {
        client_ev.events  = EPOLLIN | EPOLLET;
        client_ev.data.fd = clientFd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &client_ev) < 0)
        {
            delete client;
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, LOG_ADD_CLIENT_EPOLL_FAIL, __FUNCTION__);
        }
    }
    return (client);
}

int ListeningSocket::getPort(void) const { return ntohs(_ListeningAddr.sin_port); }

std::string ListeningSocket::getAddress(void) const
{
    uint32_t           ip;
    unsigned char      bytes[4];
    std::ostringstream oss;

    // Convert network byte order to host byte order
    ip = ntohl(_ListeningAddr.sin_addr.s_addr);

    // Extract each byte
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    // Convert to string using C++ streams
    oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1]) << "."
        << static_cast<int>(bytes[2]) << "." << static_cast<int>(bytes[3]);

    return oss.str();
}
