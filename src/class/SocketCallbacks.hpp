/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketCallbacks.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETCALLBACKS_HPP
#define SOCKETCALLBACKS_HPP

#include "Callback.hpp"
#include "ClientSocket.hpp"

// Forward declaration
class SocketManager;

// Accept new connection callback
class AcceptCallback : public Callback
{
public:
    AcceptCallback(int serverFd, SocketManager* manager, int epollFd);
    virtual ~AcceptCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int _epollFd;
};

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

// Write data to client callback
class WriteCallback : public Callback
{
public:
    WriteCallback(int clientFd, SocketManager* manager, const std::string& data);
    virtual ~WriteCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    std::string _data;
};

// Error/cleanup callback
class ErrorCallback : public Callback
{
public:
    ErrorCallback(int fd, SocketManager* manager, int epollFd);
    virtual ~ErrorCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int _epollFd;
};

// Timeout callback
class TimeoutCallback : public Callback
{
public:
    TimeoutCallback(int fd, SocketManager* manager, int epollFd);
    virtual ~TimeoutCallback();
    virtual void execute();

private:
    SocketManager* _manager;
    int _epollFd;
};

#endif  // ********************************************** SOCKETCALLBACKS_HPP //

