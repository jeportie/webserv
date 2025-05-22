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

#include "../SocketManager.hpp"
#include "../ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "ReadCallback.hpp"
#include "ErrorCallback.hpp"

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
    std::ostringstream oss;

    try
    {
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
