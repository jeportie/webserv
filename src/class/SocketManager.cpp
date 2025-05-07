/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:44:31 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <ostream>
#include "SocketManager.hpp"

SocketManager::SocketManager(void)
{
	std::cout << "[SocketManager] - default constructor called - " << std::endl;
}

SocketManager::SocketManager(const SocketManager& src)
{
	std::cout << "[SocketManager] - copy constructor called - " << std::endl;
	*this = src;
	return;
}

SocketManager::~SocketManager(void)
{
	std::cout << "[SocketManager] - destructor called - " << std::endl;
	return;
}

SocketManager & SocketManager::operator=(const SocketManager& rhs)
{
	std::cout << "[SocketManager] - copy assignment operator called - " << std::endl;
//	if (this != &rhs)
//		this->_foo = rhs.getFoo();
	return (*this);
}

int safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd
                  << ", fd=" << fd << ", op=" << op << "): "
                  << strerror(errno) << std::endl;
        return -1;
    }
    return 0;
}

void Server::safeRegisterToEpoll(int epoll_fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _serverSocketFd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
		throw std::runtime_error("Failed to add server socket to epoll: " + std::string(strerror(errno)));
}

void Server::safeAccept(int epoll_fd)
{
    _clientSocketFd = accept(_serverSocketFd, (struct sockaddr*) &_clientSockAdr, &_clientSockAdrLen);
    if (_clientSocketFd < 0)
        throw std::runtime_error("Accept() Failed");
        
    std::cout << "Connection accepted!\n";
    setNonBlockingClient(_clientSocketFd);
    
    epoll_event client_ev;
    client_ev.events  = EPOLLIN | EPOLLET;
    client_ev.data.fd = _clientSocketFd;
    
 
