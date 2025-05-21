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
#include "ErrorHandler.hpp"

CallbackQueue::CallbackQueue()
{
}

CallbackQueue::~CallbackQueue()
{
    // Clean up any remaining callbacks
    while (!_queue.empty())
    {
        Callback* callback = _queue.front();
        _queue.pop();
        delete callback;
    }
}

void CallbackQueue::push(Callback* callback)
{
    const size_t MAX_QUEUE_SIZE = 1000;  // Adjust as needed

    if (_queue.size() >= MAX_QUEUE_SIZE)
    {
        // Queue is full, reject the callback
        delete callback;
        return;
    }
    _queue.push(callback);
}

void CallbackQueue::processCallbacks()
{
    while (!_queue.empty())
    {
        Callback* callback = _queue.front();
        _queue.pop();

        try
        {
            if (callback) // Check if callback is valid
            {
                callback->execute();
            }
            else
            {
                ErrorHandler::getInstance().logError(
                    WARNING, CALLBACK_ERROR, "Null callback in queue", "CallbackQueue::processCallbacks");
                continue; // Skip to next callback
            }
        }
        catch (const std::exception& e)
        {
            std::string errorMsg = "Error executing callback: ";
            errorMsg += e.what();
            ErrorHandler::getInstance().logError(
                ERROR, CALLBACK_ERROR, errorMsg, "CallbackQueue::processCallbacks");
        }
        catch (...)
        {
            ErrorHandler::getInstance().logError(ERROR,
                                                 CALLBACK_ERROR,
                                                 "Unknown error executing callback",
                                                 "CallbackQueue::processCallbacks");
        }

        delete callback; // Always delete the callback after execution
    }
}

bool CallbackQueue::isEmpty() const
{
    return _queue.empty();
}

size_t CallbackQueue::size() const
{
    return _queue.size();
}

