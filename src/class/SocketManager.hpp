/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 13:52:48 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <arpa/inet.h>
#include <map>
#include <sys/epoll.h>
#include <queue>
#include "CallbackQueue.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Timer.hpp"
#include "Callback.hpp"

// Forward declarations
class Callback;

class SocketManager
{
public:
    SocketManager(void);
    ~SocketManager(void);

    void init_connect(void);
    void eventLoop(int epoll_fd, int timeout_ms = -1);
    int  calculateEpollTimeout(int timeout_ms);
    int  addTimer(int seconds, Callback* callback);
    bool cancelTimer(int fd);
    void processTimers();
    void cleanupClientSocket(int fd, int epoll_fd);
    void safeRegisterToEpoll(int epoll_fd);
    void addClientSocket(int fd, ClientSocket* client);

    ServerSocket& getServerSocket();
    CallbackQueue& getCallbackQueue();

private:
    SocketManager(const SocketManager& src);
    SocketManager& operator=(const SocketManager& rhs);

    ServerSocket                 _serverSocket;     ///< The server socket
    std::map<int, ClientSocket*> _clientSockets;    ///< Map of client sockets by file descriptor
    int                          _serverSocketFd;   ///< Server socket file descriptor
    std::priority_queue<Timer>   _timerQueue;       ///< Priority queue of timers
    CallbackQueue          _callbackQueue;    ///< Simple callback queue
};

#endif  // ************************************************ SOCKETMANAGER_HPP //
