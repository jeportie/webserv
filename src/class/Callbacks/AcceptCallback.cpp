/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AcceptCallback.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:42:25 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 12:48:50 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "AcceptCallback.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// AcceptCallback implementation
AcceptCallback::AcceptCallback(int serverFd, SocketManager* manager, int epollFd)
: Callback(serverFd)
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
    ClientSocket*		client;
    int					clientFd;
    std::string			msg;

    while (true)
    {
        try
        {
            client = _manager->getServerSocket().safeAccept(_epollFd);
			if (!client)
				return ;
            clientFd = client->getFd();
            _manager->addClientSocket(clientFd, client);

            client->touch(); // Init timer for the new client;

            oss << "New connection from " << client->getClientIP() << ":"
                << client->getClientPort() << " (fd=" << clientFd << ")" << std::endl;
			std::cout << oss.str();
			LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
        }
        catch (const std::exception& e)
        {
            msg = e.what();
            if (msg.find("Resource temporarily unavailable") != std::string::npos)
            {
                // No more clients to accept (EAGAIN/EWOULDBLOCK)
                break;
            }
            LOG_ERROR(ERROR, SOCKET_ERROR, "Accept failed: " + msg, __FUNCTION__);
            break;
        }
    }
}
