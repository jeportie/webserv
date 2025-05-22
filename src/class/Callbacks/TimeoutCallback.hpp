/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeoutCallback.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:12:37 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:53:05 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMEOUTCALLBACK_HPP
#define TIMEOUTCALLBACK_HPP

#include "Callback.hpp"

class SocketManager;

// Timeout callback
class TimeoutCallback : public Callback
{
public:
    TimeoutCallback(int fd, SocketManager* manager, int epollFd);
    virtual ~TimeoutCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int            _epollFd;
};

#endif  // ********************************************** TIMEOUTCALLBACK_HPP //
