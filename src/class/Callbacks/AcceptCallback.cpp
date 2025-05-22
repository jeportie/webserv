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

#include "../SocketManager.hpp"
#include "../ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "AcceptCallback.hpp"

#include <unistd.h>
#include <sys/epoll.h>

// AcceptCallback implementation
AcceptCallback::AcceptCallback(int serverFd, SocketManager* manager, int epollFd)
: Callback(serverFd)
, _manager(manager)
, _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "AcceptCallback Constructor called.",
	 	"AcceptCallback::AcceptCallback(int fd)");
}

AcceptCallback::~AcceptCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "AcceptCallback Destructor called.",
	 	   "AcceptCallback::~AcceptCallback()");
}

void AcceptCallback::execute()
{
    ClientSocket* client;
    int clientFd;

	try
    {
        client = _manager->getServerSocket().safeAccept(_epollFd);
        if (client)
        {
            clientFd = client->getFd();
            _manager->addClientSocket(clientFd, client);

            client->touch(); // Init timer for the new client;

            std::cout << "New connection from " << client->getClientIP() << ":"
                      << client->getClientPort() << " (fd=" << clientFd << ")" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(ERROR, SOCKET_ERROR, "Accept failed: " + std::string(e.what()),
				"AcceptCallback::execute");
    }
}
