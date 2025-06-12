/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:07:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/12 08:54:09 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../include/webserv.hpp"
#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpRequest.hpp"
#include "../Http/RequestValidator.hpp"
#include "../Http/HttpResponseBuilder.hpp"
#include "../Http/ResponseFormatter.hpp"
#include "ReadCallback.hpp"
#include "ErrorCallback.hpp"
#include "CloseCallback.hpp"
#include "WriteCallback.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>

// ReadCallback implementation
ReadCallback::ReadCallback(int clientFd, SocketManager* manager, int epoll_fd)
: Callback(clientFd)
, _manager(manager)
, _epoll_fd(epoll_fd)
{
    LOG_ERROR(DEBUG,
              CALLBACK_ERROR,
              "ReadCallback Constructor called.",
              "ReadCallback::ReadCallback(int fd)");
}

ReadCallback::~ReadCallback()
{
    LOG_ERROR(
        DEBUG, CALLBACK_ERROR, "ReadCallback Destructor called.", "ReadCallback::~ReadCallback()");
}

void ReadCallback::execute()
{
    ClientSocket*      client;
    std::ostringstream oss;

    client                                  = NULL;
    const std::map<int, ClientSocket*>& map = _manager->getClientMap();
    // std::cout << "fd : " << _fd << std::endl;
    std::map<int, ClientSocket*>::const_iterator it = map.find(_fd);
    (it != map.end()) ? client = it->second : NULL;
    if (!client)
    {
        // Cas normal si la socket a été récemment close
        // Tu peux commenter cette ligne ou mettre en debug uniquement
        // LOG_ERROR(ERROR, CALLBACK_ERROR, "ClientSocket not found for fd",
        // "ReadCallback::execute"); _manager->getCallbackQueue().push(new _fd,
        // _manager, -1, SOFT));
        return;
    }

    try
    {
        if (!readFromClient(_fd, client))
            return;

        // std::cout << client->requestData.getBuffer() << std::endl;
        // debug

        if (client->requestData.getBuffer().empty())
            throw HttpException(400, "Bad Request: Empty request buffer", "");

        if (!parseClientHeaders(client))
            return;

        if (!parseClientBody(client))
            return;

        // init config server des maintenant pour recuperer error page en cas de throw que une fois
        // que la requete est complete et prete a etre build
        HttpRequest req = buildHttpRequest(client);
        client->requestData.initServerConfig(_manager->getConfiguration());
        ServerConfig     config = client->requestData.getServerConfig();  // copie
        RequestValidator validator(req, config);
        validator.validate();  // peut lancer HttpException si erreur de validation

        HttpResponseBuilder builder(req, validator);
        builder.buildResponse();

        const HttpResponse& resp = builder.getResponse();
        ResponseFormatter   formatter(resp);
        std::string         headers = formatter.formatHeadersOnly(resp);

        struct epoll_event ev;
        ev.events  = EPOLLOUT | EPOLLET;
        ev.data.fd = _fd;
        _manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);

        // au lieu de push le new writecallback initialiser a la reponse quon veut on va le push sur
        // une waiting list ;
        if (!resp.getFileToStream().empty())
        {
            int file_fd = open(resp.getFileToStream().c_str(), O_RDONLY);
            if (file_fd < 0)
            {
                throw HttpException(
                    500, "Internal Server Error: open() failed", validator.getErrorPage(500));
            }
            _manager->getWaitingList()[_fd].push(
                new WriteCallback(_fd, _manager, headers, file_fd, _epoll_fd));
        }
        else
        {
            _manager->getWaitingList()[_fd].push(
                new WriteCallback(_fd, _manager, headers, resp.getBody(), _epoll_fd));
        }

        cleanupRequest(client);
    }
    catch (const HttpException& he)
    {
        if (he.customPage().empty())
        {
            struct epoll_event ev;
            ev.events  = EPOLLOUT | EPOLLET ;
            ev.data.fd = _fd;
            _manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
            sendErrorResponse(_fd, he.status(), he.what());
        }
        else
        {
            struct epoll_event ev;
            ev.events  = EPOLLOUT | EPOLLET;
            ev.data.fd = _fd;
            _manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
            sendCustomErrorResponse(_fd, he.status(), he.customPage());
        }
        // retire de epoll, delete ClientSocket, close(fd)
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _manager->getEpollFd(), SOFT));

        return;  // on arrête là pour ce fd
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, e.what(), "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _manager->getEpollFd(), FATAL));
    }
    catch (...)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, "Unknown error", "ReadCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _manager->getEpollFd(), FATAL));
    }
}
