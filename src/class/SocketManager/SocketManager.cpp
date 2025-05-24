/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:21:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "SocketManager.hpp"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../../../include/webserv.h"

SocketManager::SocketManager(void)
: _serverSocketFd(-1)
, _clientSocketFd(-1)
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "SocketManager Constructor called.", "SocketManager::SocketManager()");
}

SocketManager::~SocketManager(void)
{
	std::map<int, ClientSocket*>::iterator it;

    LOG_ERROR(
        DEBUG, SOCKET_ERROR, "SocketManager Destructor called.", "SocketManager::~SocketManager()");

    for (it = _clientSockets.begin(); it != _clientSockets.end(); ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}
