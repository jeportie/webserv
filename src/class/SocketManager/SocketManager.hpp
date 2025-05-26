/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/26 12:36:38 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
#define SOCKETMANAGER_HPP

#define LOG_SM_CONST "SocketManager Constructor called"
#define LOG_SM_DEST "SocketManager Destructor called"
#define LOG_SM_EPOLL "Failed to add server socket to epoll"

#include <arpa/inet.h>
#include <sys/epoll.h>

#include "../Sockets/ClientSocket.hpp"
#include "../Callbacks/CallbackQueue.hpp"
#include "../../../include/webserv.h"

class Callback;

class SocketManager
{
public:
    SocketManager(void);
    ~SocketManager(void);

    void	init_connect();
    void	eventLoop(int epoll_fd);
    void	addClientSocket(int fd, ClientSocket* client);
    void	cleanupClientSocket(int fd, int epoll_fd);
    void	enqueueReadyCallbacks(int n, EVENT_LIST& events, int epoll_fd);
	bool	isListeningSocket(int fd) const;
    void	scanClientTimeouts(int epoll_fd);
    void	instantiateConfig(const std::string& content);
    int 	safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event);
    void	safeRegisterToEpoll(int epoll_fd, int serverFd);
	IVSCMAP	ReadandParseConfigFile(const std::string& content);

    SSVECTOR&		getServerSocket();
    CallbackQueue&	getCallbackQueue();
    int				getCheckIntervalMs(void);
    int				getClientSocketFd(void) const;
    IVSCMAP			getConfiguration(void) const { return _serversByPort; }

    int setNonBlockingServer(int fd);
    

    const ICMAP& getClientMap(void) const;

private:
    SocketManager(const SocketManager& src);
    SocketManager& operator=(const SocketManager& rhs);

    ICMAP         _clientSockets;   ///< Map of client sockets by file descriptor
	SSVECTOR	  _serverSockets;  ///< The server socket
    int           _clientSocketFd;  ///< Client socket file descriptor (most recent)
    CallbackQueue _callbackQueue;   ///< Simple callback queue
    IVSCMAP        _serversByPort;
};

#endif  // ************************************************ SOCKETMANAGER_HPP //
