/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/16 15:38:08 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <arpa/inet.h>
# include <map>
# include <sys/epoll.h>
# include "ServerSocket.hpp"
#include "HttpRequest.hpp"
# include "ClientSocket.hpp"

/**
 * @brief Manages server and client sockets
 * 
 * This class is responsible for managing server sockets and client connections.
 * It handles the creation of server sockets, accepting client connections,
 * and managing the event loop for handling I/O events.
 */
class SocketManager
{
public:

    SocketManager(void);
    SocketManager(const SocketManager& src);
    ~SocketManager(void);
    SocketManager& operator=(const SocketManager& rhs);

    /**
     * @brief Initializes the server connection
     * 
     * Creates a server socket, binds it to a port, and starts listening
     * for connections. Then creates an epoll instance and registers the
     * server socket with it.
     */
    void init_connect(void);

    /**
     * @brief Handles communication with a client
     * 
     * Reads data from a client socket and processes it.
     * 
     * @param fd The client socket file descriptor
     */
    bool communication(int fd);


    /**
     * @brief Runs the event loop
     * 
     * Waits for events on the registered file descriptors and handles them.
     * 
     * @param epoll_fd The epoll file descriptor
     */
    void eventLoop(int epoll_fd);

    /**
     * @brief Sets the server socket to non-blocking mode
     * 
     * @param fd The file descriptor to set to non-blocking mode
     * @return int Return value from fcntl call, -1 on error
     */
    int setNonBlockingServer(int fd);
    
    /**
     * @brief Safely registers a file descriptor with epoll
     * 
     * @param epoll_fd The epoll file descriptor
     * @param op The operation to perform (EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL)
     * @param fd The file descriptor to register
     * @param event The epoll_event structure
     * @return int 0 on success, -1 on error
     */
    int safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event);
    
    /**
     * @brief Registers the server socket with epoll
     * 
     * @param epoll_fd The epoll file descriptor
     */
    void safeRegisterToEpoll(int epoll_fd);

    int getServerSocket(void) const;
    int getClientSocket(void) const;

    void closeConnection(int  fd, int epoll_fd);
    
    
    
    
    
    
    
    private:
    
    bool readFromClient(int fd);
    bool parseClientHeaders(ClientSocket* client);
    bool parseClientBody(ClientSocket* client);
    HttpRequest buildHttpRequest(ClientSocket* client);
    void handleHttpRequest(int fd, HttpRequest& req);
    void cleanupRequest(ClientSocket* client);
    ServerSocket				 _serverSocket;	  ///< The server socket
    std::map<int, ClientSocket*> _clientSockets;  ///< Map of client sockets by file descriptor

    int							 _serverSocketFd; ///< Server socket file descriptor
    int							 _clientSocketFd; ///< Client socket file descriptor (most recent)
};

#endif  // ************************************************ SOCKETMANAGER_HPP //

