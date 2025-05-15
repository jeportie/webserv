/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackManager.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 12:09:03 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 12:13:37 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACKMANAGER_HPP
# define CALLBACKMANAGER_HPP

#include "CallbackQueue.hpp"
#include <map>

/**
 * @brief Manages different types of callbacks
 * 
 * This class manages different types of callbacks (immediate, deferred)
 * and provides error handling and lifecycle management.
 */
class CallbackManager 
{
public:
    // Callback priority levels
    enum Priority
    {
        HIGH,
        NORMAL,
        LOW
    };
    
    CallbackManager();
    ~CallbackManager();
    
    /**
     * @brief Add a callback to be executed immediately
     * 
     * @param callback The callback to execute
     */
    void executeImmediate(Callback* callback);
    
    /**
     * @brief Add a callback to be executed later
     * 
     * @param callback The callback to execute
     * @param priority The priority of the callback
     */
    bool executeDeferred(Callback* callback, Priority priority = NORMAL);
    
    /**
     * @brief Process all deferred callbacks
     * 
     * Executes all deferred callbacks in priority order.
     */
    void processDeferredCallbacks();
    
    /**
     * @brief Cancel all callbacks for a specific file descriptor
     * 
     * @param fd The file descriptor to cancel callbacks for
     * @return int Number of callbacks cancelled
     */
    int cancelCallbacksForFd(int fd);
    
    /**
     * @brief Check if there are any pending callbacks
     * 
     * @return bool True if there are pending callbacks
     */
    bool hasPendingCallbacks() const;
    
private:
    std::map<Priority, CallbackQueue> _deferredCallbacks;  // Queues for different priority levels
    
    // Prevent copying
    // CallbackManager(const CallbackManager& src);
};

#endif  // ********************************************** CALLBACKMANAGER_HPP //
