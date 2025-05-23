/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:07:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:52:01 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "../Http/HttpException.hpp"
#include "ReadCallback.hpp"
#include "ErrorCallback.hpp"
#include "../Http/RequestLine.hpp"
#include "../Http/HttpRequest.hpp"



#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// ReadCallback implementation
ReadCallback::ReadCallback(int clientFd, SocketManager* manager)
: Callback(clientFd)
, _manager(manager)
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

    const std::map<int, ClientSocket*>& map = _manager->getClientMap();
    std::map<int, ClientSocket*>::const_iterator it = map.find(_fd);
    (it != map.end()) ? client = it->second : NULL;

    
    try
    {
    if (!readFromClient(_fd))
        return ;

    if (!parseClientHeaders(client))
        return ;

    if (parseClientBody(client))
        return ;

    HttpRequest req = buildHttpRequest(client);

    // handleHttpRequest(fd, req);

    cleanupRequest(client);

    req.headers.count("Connection") && req.headers["Connection"][0] == "close")
    {
        return false;  // plus de traitement sur cette socket
                           // closeConnection(fd, epoll_fd); REPLACER DASN EVENT LOOOP
    }



    
    
    
        // communication() returns false if the socket should be closed
        if (!_manager->communication(_fd))
        {
            oss << _fd;
            LOG_ERROR(INFO, SOCKET_ERROR,
				"Closing client connection (fd=" + oss.str() + ")",
                "ReadCallback::execute");
            _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
        }
    }
    catch (const HttpException& he)
    {
        // Erreur de la requête (4xx / 5xx) :
        sendErrorResponse(_fd, he.status(), he.what());
        // closeConnection(fd, epoll_fd);       // retire de epoll, delete ClientSocket, close(fd)
        return false;  // on arrête là pour ce fd
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


