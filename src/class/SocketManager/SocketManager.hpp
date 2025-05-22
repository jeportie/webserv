/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 11:35:29 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#include <arpa/inet.h>
#include <map>
#include <sys/epoll.h>

#include "../Sockets/ServerSocket.hpp"
#include "../Http/HttpRequest.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../Callbacks/CallbackQueue.hpp"


class Callback;

class SocketManager
{
public:
    SocketManager(void);
    ~SocketManager(void);

    void init_connect(void);
    void eventLoop(int epoll_fd);
    void cleanupClientSocket(int fd, int epoll_fd);

    bool communication(int fd);
    int  setNonBlockingServer(int fd);
    int  safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event);
    void safeRegisterToEpoll(int epoll_fd);
    void addClientSocket(int fd, ClientSocket* client);

    ServerSocket&  getServerSocket();
    CallbackQueue& getCallbackQueue();

    int  getCheckIntervalMs(void);
    void enqueueReadyCallbacks(int n, std::vector<epoll_event>& events, int epoll_fd);
    void scanClientTimeouts(int epoll_fd);
    int  getServerSocketFd(void) const;
    int  getClientSocketFd(void) const;

private:
    SocketManager(const SocketManager& src);
    SocketManager& operator=(const SocketManager& rhs);

    ServerSocket                 _serverSocket;    ///< The server socket
    std::map<int, ClientSocket*> _clientSockets;   ///< Map of client sockets by file descriptor
    int                          _serverSocketFd;  ///< Server socket file descriptor
    int                          _clientSocketFd;  ///< Client socket file descriptor (most recent)
    CallbackQueue                _callbackQueue;   ///< Simple callback queue


    void        closeConnection(int fd, int epoll_fd);
    bool        readFromClient(int fd);
    bool        parseClientHeaders(ClientSocket* client);
    bool        parseClientBody(ClientSocket* client);
    HttpRequest buildHttpRequest(ClientSocket* client);
    void        handleHttpRequest(int fd, HttpRequest& req);
    void        cleanupRequest(ClientSocket* client);
};

#endif  // ************************************************ SOCKETMANAGER_HPP //
