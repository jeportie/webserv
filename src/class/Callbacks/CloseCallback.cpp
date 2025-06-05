/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 11:35:53 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/05 12:28:00 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "CloseCallback.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../SocketManager/SocketManager.hpp"
#include "../../../include/webserv.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>

// CloseCallback implementation
CloseCallback::CloseCallback(int fd, SocketManager* manager, int epollFd)
: Callback(fd), _manager(manager), _epollFd(epollFd)
{
	 LOG_ERROR(DEBUG, CALLBACK_CLOSE, LOG_CLSCB_CONST, __FUNCTION__);
}

CloseCallback::~CloseCallback()
{
	 LOG_ERROR(DEBUG, CALLBACK_CLOSE, LOG_CLSCB_DEST, __FUNCTION__);
}

void CloseCallback::execute()
{
    _manager->cleanupClientSocket(_fd, _epollFd);
}
