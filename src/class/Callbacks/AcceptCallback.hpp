/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AcceptCallback.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:37:37 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 12:50:19 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCEPTCALLBACK_HPP
#define ACCEPTCALLBACK_HPP

#define LOG_ACCEPT_CALLB_CONST	"AcceptCallback Constructor called"
#define LOG_ACCEPT_CALLB_DEST	"AcceptCallback Destructor called"

#include "Callback.hpp"

class SocketManager;

class AcceptCallback : public Callback
{
public:
    AcceptCallback(int serverFd, SocketManager* manager, int epollFd);
    virtual ~AcceptCallback();

    virtual void execute();

private:
    SocketManager* _manager;
    int            _epollFd;
};

#endif  // *********************************************** ACCEPTCALLBACK_HPP //
