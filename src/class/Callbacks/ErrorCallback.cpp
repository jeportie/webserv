/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:13:44 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/05 12:14:29 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorCallback.hpp"
#include "CloseCallback.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../SocketManager/SocketManager.hpp"
#include "../../../include/webserv.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>


// ErrorCallback implementation
ErrorCallback::ErrorCallback(int fd, SocketManager* manager, int epollFd, bool is_fatal)
: Callback(fd), _manager(manager), _epollFd(epollFd), _is_fatal(is_fatal)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ERRCB_CONST, __FUNCTION__);
}

ErrorCallback::~ErrorCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_ERRCB_DEST, __FUNCTION__);
}

void ErrorCallback::execute()
{
	if (_is_fatal)
	{
		_manager->getCallbackQueue().push(new CloseCallback(_fd, _manager, _epollFd));
	}
}
