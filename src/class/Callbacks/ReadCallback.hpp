/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:59:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:52:18 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef READCALLBACK_HPP
# define READCALLBACK_HPP

#include "Callback.hpp"

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

    
    void        closeConnection(int fd, int epoll_fd);
    bool        readFromClient(int fd);
    bool        parseClientHeaders(ClientSocket* client);
    bool        parseClientBody(ClientSocket* client);
    HttpRequest buildHttpRequest(ClientSocket* client);
    void        handleHttpRequest(int fd, HttpRequest& req);
    void        cleanupRequest(ClientSocket* client);
};

#endif  // ************************************************* READCALLBACK_HPP //
