/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:07:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/30 18:08:50 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "../Http/HttpException.hpp"
#include "ReadCallback.hpp"
#include "ErrorCallback.hpp"
#include "../Http/HttpRequest.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// ReadCallback implementation
ReadCallback::ReadCallback(int clientFd, SocketManager* manager, int epoll_fd)
: Callback(clientFd)
, _manager(manager)
, _epoll_fd(epoll_fd)
{
    LOG_ERROR(DEBUG, CALLBACK_ERROR, "ReadCallback Constructor called.",
		"ReadCallback::ReadCallback(int fd)");
}

ReadCallback::~ReadCallback()
{
    LOG_ERROR(DEBUG, CALLBACK_ERROR, "ReadCallback Destructor called.",
			"ReadCallback::~ReadCallback()");
}

void ReadCallback::execute()
{
    ClientSocket* client;
    std::ostringstream oss;

	client = NULL;
    const std::map<int, ClientSocket*>& map = _manager->getClientMap();
    std::map<int, ClientSocket*>::const_iterator it = map.find(_fd);
    (it != map.end()) ? client = it->second : NULL;
    if (!client)
	{
        LOG_ERROR(ERROR, CALLBACK_ERROR, "ClientSocket not found for fd", "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
        return;
    }

    try
    {
        if (!readFromClient(_fd, client))
            return ;
        
        std::cout << client->requestData.getBuffer() << std::endl;
        //pourquoi ce print ?
        if (client->requestData.getBuffer().empty())
            throw HttpException(400, "Bad Request: Empty request buffer");
            
        if (!parseClientHeaders(client))
            return ;
        
        if (parseClientBody(client))
            return ;

        client->requestData.initServerConfig(_manager->getConfiguration());
        HttpRequest req = buildHttpRequest(client);
        
        

        // handleHttpRequest(fd, req);

        cleanupRequest(client);
        if (req.headers.count("Connection") && req.headers["Connection"][0] == "close")
        {
            oss << _fd;
            LOG_ERROR(INFO, SOCKET_ERROR,
                "Closing client connection (fd=" + oss.str() + ")",
                "ReadCallback::execute");
                _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
                return ;  // plus de traitement sur cette socket
        }
    }
    catch (const HttpException& he)
    {
        // Erreur de la requête (4xx / 5xx) :
        sendErrorResponse(_fd, he.status(), he.what());

		// retire de epoll, delete ClientSocket, close(fd)
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));

        return ;  // on arrête là pour ce fd
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, e.what(), "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
    catch (...)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, "Unknown error", "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
}

/* TODO:
mettre le sendErrorResponde des requete Http en cas de soucis dans la requete dans le Error pour suivre la meme logique consistant a mettre un evenement dans la queu et de la traiter apres.
*/