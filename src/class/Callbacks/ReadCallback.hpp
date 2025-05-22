/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:59:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:06:59 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READCALLBACK_HPP
# define READCALLBACK_HPP

#include "Callback.hpp"

// Forward declaration
class SocketManager;

// Read data from client callback
class ReadCallback : public Callback
{
public:
    ReadCallback(int clientFd, SocketManager* manager);
    virtual ~ReadCallback();
    virtual void execute();

private:
    SocketManager* _manager;
};

#endif  // ************************************************* READCALLBACK_HPP //
