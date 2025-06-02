/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:51:25 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "CallbackQueue.hpp"
#include "../../../include/webserv.hpp"

CallbackQueue::CallbackQueue()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_CBQUEU_CONST, __FUNCTION__);
}

CallbackQueue::~CallbackQueue()
{
    Callback* callback; 

	LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_CBQUEU_DEST, __FUNCTION__);

    // Clean up any remaining callbacks
    while (!_queue.empty())
    {
        callback = _queue.front();
        _queue.pop();
        delete callback;
    }
}

