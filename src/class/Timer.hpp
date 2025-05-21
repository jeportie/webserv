/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:17:36 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMER_HPP
# define TIMER_HPP

#include "Callback.hpp"
#include <ctime>

class Timer
{
public:
	Timer(time_t expireTime, Callback* callback);
	Timer(const Timer& src);
	Timer& operator=(const Timer& rhs);
	~Timer(void);

	time_t		getExpireTime(void) const;
	Callback*	getCallback(void) const;
	void		setCallback(Callback* callback);
	int         getTimerFd(void) const;  // Safe way to get the fd without accessing callback directly

    bool operator<(const Timer& src) const;

private:
	time_t		_expireTime;
	Callback*	_callback; // callback to execute when timer expires
	bool		_ownsCallback; // Flag to indicate if this Timer owns the callback
};

#endif  // ******************************************************** TIMER_HPP //

