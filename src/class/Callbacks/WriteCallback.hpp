/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:55:20 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 12:35:33 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WRITECALLBACK_HPP
# define WRITECALLBACK_HPP

#include "Callback.hpp"
#include <string>

class SocketManager;

// Write data to client callback
class WriteCallback : public Callback
{
public:
    WriteCallback(int clientFd, SocketManager* manager, const std::string& data, int epoll_fd);
    virtual ~WriteCallback();

    virtual void execute();

private:
    SocketManager* _manager;
    std::string    _data;
    int            _epoll_fd;
};

#endif  // ************************************************ WRITECALLBACK_HPP //
