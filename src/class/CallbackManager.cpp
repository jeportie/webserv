/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 12:14:14 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 12:36:38 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CallbackManager.hpp"
#include "ErrorHandler.hpp"

CallbackManager::CallbackManager()
{
    // Initialize the callback queues for each priority level
    _deferredCallbacks[HIGH] = CallbackQueue();
    _deferredCallbacks[NORMAL] = CallbackQueue();
    _deferredCallbacks[LOW] = CallbackQueue();
}

// CallbackManager::CallbackManager(const CallbackManager& src) { *this = src; }

CallbackManager::~CallbackManager(void) {}


void CallbackManager::executeImmediate(Callback* callback)
{
    try
    {
        if (!callback->isCancelled())
        {
            callback->execute();
        }
    }
    catch (const std::exception& e)
    {
        std::string errorMsg = "Error executing immediate callback: ";
        errorMsg += e.what();
        ErrorHandler::getInstance().logError(ERROR, CALLBACK_ERROR, errorMsg, "CallbackManager::executeImmediate");
    }
    catch (...)
    {
        ErrorHandler::getInstance().logError(ERROR, CALLBACK_ERROR, "Unknown error executing immediate callback", "CallbackManager::executeImmediate");
    }
    
    delete callback;
}

bool CallbackManager::executeDeferred(Callback* callback, Priority priority)
{
    return (_deferredCallbacks[priority].addCallback(callback));
}

void CallbackManager::processDeferredCallbacks()
{
    // Process callbacks in priority order (HIGH to LOW)
    _deferredCallbacks[HIGH].processCallbacks();
    _deferredCallbacks[NORMAL].processCallbacks();
    _deferredCallbacks[LOW].processCallbacks();
}

int CallbackManager::cancelCallbacksForFd(int fd)
{
    int count = 0;
    
    // Cancel callbacks in all priority queues
    count += _deferredCallbacks[HIGH].cancelCallbacksForFd(fd);
    count += _deferredCallbacks[NORMAL].cancelCallbacksForFd(fd);
    count += _deferredCallbacks[LOW].cancelCallbacksForFd(fd);
    
    return count;
}

bool CallbackManager::hasPendingCallbacks() const
{
    return !_deferredCallbacks.at(HIGH).isEmpty() ||
           !_deferredCallbacks.at(NORMAL).isEmpty() ||
           !_deferredCallbacks.at(LOW).isEmpty();
}
