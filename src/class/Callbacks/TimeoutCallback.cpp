/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutCallback.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:16:41 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:52:50 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeoutCallback.hpp"
#include "../SocketManager/SocketManager.hpp"
#include "../../../include/webserv.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <sstream>

// TimeoutCallback implementation
TimeoutCallback::TimeoutCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_TIMEOUTCB_CONST, __FUNCTION__);
}

TimeoutCallback::~TimeoutCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_TIMEOUTCB_DEST, __FUNCTION__);
}

void TimeoutCallback::execute()
{
	std::ostringstream	oss;
    int					fd;
    int					epollFd;

    fd = _fd;
    epollFd = _epollFd;
    
    oss << "Client connection timed out (fd=" << fd << ")" << std::endl;
	std::cout << oss.str();
	LOG_ERROR(INFO, CALLBACK_ERROR, oss.str(), __FUNCTION__);
    
    // Clean up the client socket
    _manager->cleanupClientSocket(fd, epollFd);
}

