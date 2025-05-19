/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:48:16 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/13 15:23:16 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Timer.hpp"

Timer::Timer(time_t expireTime, Callback* callback)
: _expireTime(expireTime)
, _callback(callback)
{}

Timer::~Timer() { delete _callback; }

time_t Timer::getExpireTime() const { return _expireTime; }

Callback* Timer::getCallback() const { return _callback; }

// Reverse comparison for priority queue (earliest expiration at top)
bool Timer::operator<(const Timer& src) const { return _expireTime > src._expireTime; }
