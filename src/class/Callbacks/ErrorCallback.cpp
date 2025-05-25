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
#include "../SocketManager/SocketManager.hpp"
#include "../../../include/webserv.h"

#include <unistd.h>
#include <sys/epoll.h>

// ErrorCallback implementation
ErrorCallback::ErrorCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ERRCB_CONST, __FUNCTION__);
}

ErrorCallback::~ErrorCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ERRCB_DEST, __FUNCTION__);
}

void ErrorCallback::execute()
{
    _manager->cleanupClientSocket(_fd, _epollFd);
}
