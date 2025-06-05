/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CloseCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 11:35:51 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/05 12:03:49 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLOSECALLBACK_HPP
#define CLOSECALLBACK_HPP

#define LOG_CLSCB_CONST "CloseCallback Constructor called."
#define LOG_CLSCB_DEST "CloseCallback Destructor called"

#include "Callback.hpp"

// Forward declaration
class SocketManager;

// Close/cleanup callback
class CloseCallback : public Callback
{
public:
    CloseCallback(int fd, SocketManager* manager, int epollFd);
    virtual ~CloseCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int            _epollFd;
};

#endif  // ************************************************ CLOSECALLBACK_HPP //
