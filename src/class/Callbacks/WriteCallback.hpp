/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:55:20 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 13:45:33 by fsalomon         ###   ########.fr       */
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
    WriteCallback(int clientFd, SocketManager* manager, const std::string& data, int epoll_fd, size_t bytesWritten);
    virtual ~WriteCallback();

    virtual void execute();

private:
    SocketManager* _manager;
    std::string    _data;
    int            _epoll_fd;
    size_t         _bytesWritten; // Track how many bytes have been written so far
};

#endif  // ************************************************ WRITECALLBACK_HPP //
