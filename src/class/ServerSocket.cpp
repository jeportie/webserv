/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:42:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 16:12:05 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Socket.hpp"
#include "ClientSocket.hpp"
#include <sys/epoll.h>


ServerSocket::ServerSocket(void)
: Socket()
{
    std::memset(&this->_serverAddr, 0, sizeof(sockaddr_in));
    std::cout << "[ServerSocket] - default constructor called - " << std::endl;
}
ServerSocket::~ServerSocket(void)
{
    std::cout << "[Server] - destructor called - " << std::endl;
    closeSocket();
}



bool ServerSocket::safeBind(int port, const std::string& adress)
{
    (void)adress;
    setNonBlocking(this->_socketFd);
    _serverAddr.sin_port = htons(port);
    _serverAddr.sin_family      = AF_INET; 
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->_socketFd, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0)
        throw std::runtime_error("Bind() Failed");
    return (true);
}


void ServerSocket::safeListen(int backlog)
{
    if (listen(this->_socketFd, backlog) < 0)
        throw std::runtime_error("Listen() Failed");
}

ClientSocket* ServerSocket::safeAccept(int epoll_fd)
{
    Socket client = new ClientSocket();
    
    client.setFd(accept(this->_socketFd, (struct sockaddr*) &(client._clientAddr), &(client._clientAddrLen)));
    if (client.getFd() < 0)
        throw std::runtime_error("Accept() Failed");
        
    std::cout << "Connection accepted!\n";
    client.setNonBlocking();
    
    return (client);
    // epoll_event client_ev;
    // client_ev.events  = EPOLLIN | EPOLLET;
    // client_ev.data.fd = client.getFd();
    
    // safeEpollCtlClient(epoll_fd, EPOLL_CTL_ADD, client.getFd(), &client_ev);
}