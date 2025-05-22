/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutCallback.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:16:41 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:18:43 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeoutCallback.hpp"
#include "../SocketManager.hpp"
#include "../../../include/webserv.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>


// TimeoutCallback implementation
TimeoutCallback::TimeoutCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "TimeoutCallback Constructor called.",
	 	"TimeoutCallback::TimeoutCallback(int fd)");
}

TimeoutCallback::~TimeoutCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "TimeoutCallback Destructor called.",
	 	   "TimeoutCallback::~TimeoutCallback()");
}

void TimeoutCallback::execute()
{
    // Store the fd locally since we'll be using it after potentially deleting objects
    int fd = _fd;
    int epollFd = _epollFd;
    
    std::cout << "Client connection timed out (fd=" << fd << ")" << std::endl;
    
    // Clean up the client socket
    _manager->cleanupClientSocket(fd, epollFd);
}

