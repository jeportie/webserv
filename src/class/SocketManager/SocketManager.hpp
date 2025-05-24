/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/24 12:55:14 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# define EVENT_LIST std::vector<epoll_event> 
# define ICMAP std::map<int, ClientSocket*>

# include <arpa/inet.h>
# include <map>
# include <sys/epoll.h>

# include "../Sockets/ServerSocket.hpp"
# include "../Sockets/ClientSocket.hpp"
# include "../Callbacks/CallbackQueue.hpp"

class Callback;

class SocketManager
{
public:
    SocketManager(void);
    ~SocketManager(void);

    void			init_connect(void);
    void			eventLoop(int epoll_fd);
    void			cleanupClientSocket(int fd, int epoll_fd);

    int				setNonBlockingServer(int fd);
    int				safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event);
    void			safeRegisterToEpoll(int epoll_fd);
    void			addClientSocket(int fd, ClientSocket* client);

    ServerSocket&	getServerSocket();
    CallbackQueue&	getCallbackQueue();

    int				getCheckIntervalMs(void);
    void			enqueueReadyCallbacks(int n, EVENT_LIST& events, int epoll_fd);
    void			scanClientTimeouts(int epoll_fd);
    int				getServerSocketFd(void) const;
    int				getClientSocketFd(void) const;

    const ICMAP& getClientMap(void) const;  

private:
    SocketManager(const SocketManager& src);
    SocketManager& operator=(const SocketManager& rhs);

    ICMAP			_clientSockets;   ///< Map of client sockets by file descriptor
    ServerSocket    _serverSocket;    ///< The server socket
    int				_serverSocketFd;  ///< Server socket file descriptor
    int				_clientSocketFd;  ///< Client socket file descriptor (most recent)
    CallbackQueue   _callbackQueue;   ///< Simple callback queue


};

#endif  // ************************************************ SOCKETMANAGER_HPP //
