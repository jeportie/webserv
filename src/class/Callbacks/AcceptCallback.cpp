/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AcceptCallback.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:42:25 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:51:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../Sockets/ListeningSocket.hpp"
#include "AcceptCallback.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// AcceptCallback implementation
AcceptCallback::AcceptCallback(int ListeningFd, SocketManager* manager, int epollFd)
: Callback(ListeningFd)
, _manager(manager)
, _epollFd(epollFd)
{
    LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ACCEPT_CALLB_CONST, __FUNCTION__);
}

AcceptCallback::~AcceptCallback()
{
    LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ACCEPT_CALLB_DEST, __FUNCTION__);
}

void AcceptCallback::execute()
{
    std::ostringstream	oss;
    int					clientFd;
    std::string			msg;
	ClientSocket*		client;
    ListeningSocket*       Listening;
    
    // Loop over all Listening sockets
    Listening = _manager->getListeningSocket(_fd);

    try
    {
        // Try to accept a new client on this Listening socket
        client = Listening->safeAccept(_epollFd);
        if (!client)
            return; // No more clients to accept on this socket
        clientFd = client->getFd();
        _manager->addClientSocket(clientFd, client);
        client->touch();  // Init timer for the new client
        oss << "New connection from " << client->getClientIP() << ":" << client->getClientPort()
        << " (fd=" << clientFd << ")" << std::endl;
            std::cout << oss.str();
        LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
    }
    catch (const std::exception& e)
    {
        msg = e.what();
        if (msg.find(LOG_ACCEPT_NO_RESOURCE) != std::string::npos)
        {
             // No more clients to accept (EAGAIN/EWOULDBLOCK)
            return;
        }
        LOG_ERROR(ERROR, SOCKET_ERROR, "Accept failed: " + msg, __FUNCTION__);
        return;
    }
}
