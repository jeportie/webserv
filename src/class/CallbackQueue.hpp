/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueue.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 09:41:36 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 13:59:52 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACKQUEUE_HPP
# define CALLBACKQUEUE_HPP

# include <queue>
# include "Callback.hpp"

/**
 * @brief Queue for managing callbacks
 * 
 * This class manages a queue of callbacks, allowing them to be
 * executed in order and providing error handling.
 */
class CallbackQueue
{
public:
	CallbackQueue(void);
	~CallbackQueue(void);

    /**
     * @brief Assignment operator
     * 
     * @param rhs The right-hand side object to assign from
     * @return CallbackQueue& Reference to this object
     */

    /**
     * @brief Add a callback to the queue
     * 
     * @param callback The callback to add
     */
	bool	addCallback(Callback* callback);

    /**
     * @brief Process all callbacks in the queue
     * 
     * Executes all callbacks in the queue, handling any errors
     * that occur during execution.
     */
	void	processCallbacks(void);

    /**
     * @brief Cancel all callbacks for a specific file descriptor
     * 
     * @param fd The file descriptor to cancel callbacks for
     * @return int Number of callbacks cancelled
     */
	int		cancelCallbacksForFd(int fd);

    /**
     * @brief Check if the queue is empty
     * 
     * @return bool True if the queue is empty
     */
	bool	isEmpty(void) const;

    /**
     * @brief Get the number of callbacks in the queue
     * 
     * @return size_t Number of callbacks
     */
	size_t	size(void) const;

private:
	std::queue<Callback*> _queue; // Queue of Callbacks
};


#endif  // ************************************************ CALLBACKQUEUE_HPP //
