/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:48:16 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Timer.hpp"

Timer::Timer(time_t expireTime, Callback* callback)
: _expireTime(expireTime)
, _callback(callback)
, _ownsCallback(true)
{
}

Timer::Timer(const Timer& src)
: _expireTime(src._expireTime)
, _callback(src._callback)
, _ownsCallback(false)
{
    // When copying, we don't take ownership of the callback
    // The original Timer will be responsible for deleting it
}

Timer& Timer::operator=(const Timer& rhs)
{
    if (this != &rhs)
    {
        // If we own the current callback, delete it
        if (_ownsCallback && _callback)
        {
            delete _callback;
        }
        
        _expireTime = rhs._expireTime;
        _callback = rhs._callback;
        _ownsCallback = false; // We don't take ownership when copying
    }
    return *this;
}

Timer::~Timer() 
{ 
    // Only delete the callback if we own it
    if (_ownsCallback && _callback)
    {
        delete _callback;
        _callback = NULL;
    }
}

time_t Timer::getExpireTime() const { return _expireTime; }

Callback* Timer::getCallback() const { return _callback; }

void Timer::setCallback(Callback* callback)
{
    // If we own the current callback, delete it
    if (_ownsCallback && _callback)
    {
        delete _callback;
    }
    
    _callback = callback;
    _ownsCallback = true;
}

// Reverse comparison for priority queue (earliest expiration at top)
bool Timer::operator<(const Timer& src) const { return _expireTime > src._expireTime; }

