/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:48:37 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "CallbackQueue.hpp"
#include "../../../include/webserv.h"

CallbackQueue::CallbackQueue()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "CallbackQueue Constructor called.",
	 	"CallbackQueue::CallbackQueue(int fd)");
}

CallbackQueue::~CallbackQueue()
{
    Callback* callback; 

	LOG_ERROR(DEBUG, CALLBACK_ERROR, "CallbackQueue Destructor called.",
	 	   "CallbackQueue::~CallbackQueue()");

    // Clean up any remaining callbacks
    while (!_queue.empty())
    {
        callback = _queue.front();
        _queue.pop();
        delete callback;
    }
}

