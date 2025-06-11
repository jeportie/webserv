/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.api.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:51:19 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CallbackQueue.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

void CallbackQueue::push(Callback* callback)
{
    if (_queue.size() >= MAX_QUEUE_SIZE)
    {
        // Queue is full, reject the callback
		LOG_SYSTEM_ERROR(ERROR, CALLBACK_ERROR, LOG_CBQUEU_FULL, __FUNCTION__);
        delete callback;
        return;
    }
    _queue.push(callback);
}

bool CallbackQueue::tryExecute(Callback* callback)
{
	std::string errorMsg;

    try
    {
        if (callback)
        {
            callback->execute();
        }
        else
        {
            LOG_ERROR(WARNING, CALLBACK_ERROR, LOG_CBQUEU_NULL, __FUNCTION__);
            return (false);  // Skip to next callback
        }
    }
    catch (const std::exception& e)
    {
        errorMsg = "Error executing callback: ";
        errorMsg += e.what();
        LOG_ERROR(ERROR, CALLBACK_ERROR, errorMsg, __FUNCTION__);
    }
    catch (...)
    {
        LOG_ERROR(ERROR, CALLBACK_ERROR, LOG_CBQUEU_UNKWN, __FUNCTION__);
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
