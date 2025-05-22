/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:13:44 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:50:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorCallback.hpp"
#include "../SocketManager.hpp"
#include "../../../include/webserv.h"

#include <unistd.h>
#include <sys/epoll.h>

// ErrorCallback implementation
ErrorCallback::ErrorCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "ErrorCallback Constructor called.",
	 	"ErrorCallback::ErrorCallback(int fd)");
}

ErrorCallback::~ErrorCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "ErrorCallback Destructor called.",
	 	   "ErrorCallback::~ErrorCallback()");
}

void ErrorCallback::execute()
{
    _manager->cleanupClientSocket(_fd, _epollFd);
}
