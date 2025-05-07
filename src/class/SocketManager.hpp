/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:33:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <iostream>
# include <vector>
# include <map>
# include <sys/epoll.h>
# include "ServerSocket.hpp"
# include "ClientSocket.hpp"

/**
 * @brief Manages server and client sockets
 * 
 * This class is responsible for creating and managing server sockets,
 * accepting new client connections, and maintaining a list of active
 * connections. It provides an interface for the application to interact
 * with sockets without dealing with low-level socket operations.
 */
class SocketManager
{
public:
    /**
     * @brief Default constructor
     */
    SocketManager(void);
    
    /**
     * @brief Destructor
     * Cleans up all server and client sockets
     */
    ~SocketManager(void);

    /**
     * @brief Creates a new server socket
     * 
     * @param port The port to bind to
     * @param address The address to bind to (default: all interfaces)
     * @return int The index of the new server socket, or -1 on failure
     */
    int createServerSocket(int port, const std::string& address = "");
    
    /**
     * @brief Initializes the epoll instance
     * 
     * @return int The epoll file descriptor, or -1 on failure
     */
    int initEpoll(void);
    
    /**
     * @brief Runs the main event loop
     * 
     * Monitors sockets for events and handles them accordingly.
     * This method blocks until an error occurs or the manager is stopped.
     */
    void eventLoop(void);
    
    /**
     * @brief Stops the event loop and cleans up resources
     */
    void stop(void);
    
    /**
     * @brief Gets the number of active client connections
     * 
     * @return size_t The number of active client connections
     */
    size_t getClientCount(void) const;
    
    /**
     * @brief Gets the number of server sockets
     * 
     * @return size_t The number of server sockets
     */
    size_t getServerCount(void) const;

private:
    /**
     * @brief Copy constructor (private to prevent copying)
     * 
     * @param src The source object to copy from
     */
    SocketManager(const SocketManager& src);
    
    /**
     * @brief Assignment operator (private to prevent assignment)
     * 
     * @param rhs The right-hand side object to assign from
     * @return SocketManager& Reference to this object
     */
    SocketManager& operator=(const SocketManager& rhs);
    
    /**
     * @brief Handles events on server sockets
     * 
     * @param serverIdx The index of the server socket
     * @return true if successful, false otherwise
     */
    bool handleServerEvent(int serverIdx);
    
    /**
     * @brief Handles events on client sockets
     * 
     * @param fd The client socket file descriptor
     * @return true if successful, false otherwise
     */
    bool handleClientEvent(int fd);
    
    /**
     * @brief Removes a client socket
     * 
     * @param fd The client socket file descriptor
     */
    void removeClient(int fd);

    std::vector<ServerSocket*> _serverSockets;  ///< List of server sockets
    std::map<int, ClientSocket*> _clientSockets;  ///< Map of client sockets (fd -> socket)
    int _epollFd;  ///< Epoll file descriptor
    bool _running;  ///< Flag indicating if the event loop is running
    static const int MAX_EVENTS = 64;  ///< Maximum number of events to process at once
    static const int LISTEN_BACKLOG = 128;  ///< Backlog for listen()
};

#endif  // ************************************************ SOCKETMANAGER_HPP //
