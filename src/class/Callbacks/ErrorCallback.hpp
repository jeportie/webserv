/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:12:07 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:12:18 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORCALLBACK_HPP
# define ERRORCALLBACK_HPP

#define LOG_ERRCB_CONST	"ErrorCallback Constructor called."
#define LOG_ERRCB_DEST	"ErrorCallback Destructor called"

#include "Callback.hpp"

// Forward declaration
class SocketManager;

// Error/cleanup callback
class ErrorCallback : public Callback
{
public:
    ErrorCallback(int fd, SocketManager* manager, int epollFd);
    virtual ~ErrorCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int            _epollFd;
};

#endif  // ************************************************ ERRORCALLBACK_HPP //
