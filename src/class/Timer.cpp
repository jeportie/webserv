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
, _ownsCallback(true) // Timer owns the callback by default
{}

Timer::Timer(const Timer& src)
: _expireTime(src._expireTime)
, _callback(src._callback ? new Callback(*src._callback) : NULL)
, _ownsCallback(src._callback != NULL) // Only own if the original had a callback
{}

Timer& Timer::operator=(const Timer& rhs) {
    if (this != &rhs) {
        _expireTime = rhs._expireTime;
        // Clean up existing callback if we own it
        if (_ownsCallback && _callback) {
            delete _callback;
        }
        // Copy the callback and update ownership
        if (rhs._callback) {
            _callback = new Callback(*rhs._callback);
            _ownsCallback = true;
        } else {
            _callback = NULL;
            _ownsCallback = false;
        }
    }
    return *this;
}

Timer::~Timer() {
    if (_ownsCallback && _callback) {
        delete _callback;
    }
}

time_t Timer::getExpireTime() const { return _expireTime; }

Callback* Timer::getCallback() const { return _callback; }

// Reverse comparison for priority queue (earliest expiration at top)
bool Timer::operator<(const Timer& src) const { return _expireTime > src._expireTime; }
