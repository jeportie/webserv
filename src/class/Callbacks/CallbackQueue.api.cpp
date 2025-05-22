/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CallbackQueue.hpp"
#include "../ErrorHandler.hpp"
#include "../ErrorHandler.hpp"
#include "../../../include/webserv.h"

void CallbackQueue::push(Callback* callback)
{
    if (_queue.size() >= MAX_QUEUE_SIZE)
    {
        // Queue is full, reject the callback
		LOG_SYSTEM_ERROR(ERROR, CALLBACK_ERROR,
				"Error: Callback Queu is full!", "CallbackQueue::push");
        delete callback;
        return;
    }
    _queue.push(callback);
}

bool CallbackQueue::tryExecute(Callback* callback)
{
    try
    {
        if (callback)
        {
            callback->execute();
        }
        else
        {
            LOG_ERROR(WARNING, CALLBACK_ERROR,
				"Null callback in queue", "CallbackQueue::processCallbacks");
            return (false);  // Skip to next callback
        }
    }
    catch (const std::exception& e)
    {
        std::string errorMsg = "Error executing callback: ";
        errorMsg += e.what();
        LOG_ERROR(ERROR, CALLBACK_ERROR, errorMsg, "CallbackQueue::processCallbacks");
    }
    catch (...)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, "Unknown error executing callback",
			"CallbackQueue::processCallbacks");
    }
	return (true);
}

void CallbackQueue::processCallbacks()
{
    Callback* callback;
	
    while (!_queue.empty())
    {
        callback = _queue.front();
        _queue.pop();

		if (!tryExecute(callback))
			continue ;
		else
			delete callback;
    }
}

bool CallbackQueue::isEmpty() const { return _queue.empty(); }

size_t CallbackQueue::size() const { return _queue.size(); }
