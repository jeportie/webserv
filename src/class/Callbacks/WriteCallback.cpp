/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:56:33 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:54:43 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"
#include "WriteCallback.hpp"
#include "ErrorCallback.hpp"

#include <unistd.h>
#include <sys/epoll.h>

// WriteCallback implementation
WriteCallback::WriteCallback(int clientFd, SocketManager* manager, const std::string& data)
: Callback(clientFd), _manager(manager), _data(data)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "WriteCallback Constructor called.",
	 	"WriteCallback::WriteCallback(int fd)");
}

WriteCallback::~WriteCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "WriteCallback Destructor called.",
	 	   "WriteCallback::~WriteCallback()");
}

void WriteCallback::execute()
{
    ssize_t written;

    written = write(_fd, _data.c_str(), _data.length());
    if (written < 0)
    {
        LOG_SYSTEM_ERROR(WARNING, SOCKET_ERROR,
			"Failed to write response to client", "WriteCallback::execute");
        _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, -1));
    }
}
