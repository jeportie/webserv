/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:17:36 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/13 15:23:23 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMER_HPP
# define TIMER_HPP

#include "Callback.hpp"
#include <ctime>

/**
 * @brief Timer class for timeout handling
 * 
 * This class represents a timer that will execute a callback
 * function when it expires. It stores the expiration time
 * and the callback to execute.
 *
 * @note How It Works
 *
 * 1. When a client connects, we create a timer that will expire in 60 seconds
 * 2. If the client sends data before the timer expires, we cancel the old timer 
 *    and create a new one
 * 3. If the timer expires, we call the callback function which closes the client connection
 * 4. In the event loop, we check for expired timers and adjust the epoll_wait 
 *    timeout based on the next timer to expire
 *
 * This timeout handling is crucial for a web server because it prevents inactive connections 
 * from consuming server resources indefinitely. Without timeouts, a malicious client could
 * open many connections and never close them, eventually causing the server to run out 
 * of resources.
 */
class Timer
{
public:
	Timer(time_t expireTime, Callback* callback);
	Timer(const Timer& src);
	Timer& operator=(const Timer& rhs);
	~Timer(void);

	time_t		getExpireTime() const;
	Callback*	getCallback() const;

    bool operator<(const Timer& src) const;

private:
	time_t		_expireTime;
	Callback*	_callback; // pointer to function if time expired
	bool        _ownsCallback; // Flag to indicate if this Timer owns the callback
};

#endif  // ******************************************************** TIMER_HPP //
