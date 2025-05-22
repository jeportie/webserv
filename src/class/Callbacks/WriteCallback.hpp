/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:55:20 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 12:56:02 by jeportie         ###   ########.fr       */
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
    WriteCallback(int clientFd, SocketManager* manager, const std::string& data);
    virtual ~WriteCallback();

    virtual void execute();

private:
    SocketManager* _manager;
    std::string    _data;
};

#endif  // ************************************************ WRITECALLBACK_HPP //
