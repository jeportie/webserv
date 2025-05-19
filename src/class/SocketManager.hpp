/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 13:52:48 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <arpa/inet.h>
# include <map>
# include <sys/epoll.h>
# include <queue>
# include "CallbackManager.hpp"
# include "ServerSocket.hpp"
#include "HttpRequest.hpp"
# include "ClientSocket.hpp"
# include "Timer.hpp"
# include "Callback.hpp"
# include "Parser.hpp"
# include "ConfigValidator.hpp"

/**
 * @brief Manages server and client sockets
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
     * @brief Initializes server connections based on configuration
     * 
     * Creates server sockets for each configured server, binds them to the specified
     * host:port combinations, and starts listening for connections. Then creates an
     * epoll instance and registers all server sockets with it.
     * 
     * @param configs Vector of server configurations
     */
    void init_connect_with_config(const std::vector<ServerConfig>& configs);

    /**
     * @brief Handles communication with a client
     * Reads data from a client socket and processes it.
     * 
     * @param fd The client socket file descriptor
     */
    bool communication(int fd);


	/**
	* @brief Static callback function for handling timeouts
	* 
	* @param fd The file descriptor that timed out
	* @param data Pointer to the SocketManager instance
	*/
	static void handleTimeout(int fd, void* data);

    /**
     * @brief Runs the event loop
     * Waits for events on the registered file descriptors and handles them.
     * 
     * @param epoll_fd The epoll file descriptor
     * @param timeout_ms Timeout in milliseconds, -1 for infinite
     */
    void eventLoop(int epoll_fd, int timeout_ms = -1);

    /**
     * @brief Adds a timer to the timer queue
     * 
     * @param seconds Seconds from now when the timer should expire
     * @param callback The callback to execute when the timer expires
     * @return int Timer ID
     */
    int addTimer(int seconds, Callback* callback);

    /**
     * @brief Cancels a timer
     * 
     * @param fd The file descriptor associated with the timer
     * @return bool True if the timer was found and cancelled
     */
    bool cancelTimer(int fd);

    /**
     * @brief Processes expired timers
     * 
     * Checks for expired timers and executes their callbacks.
     */
    void processTimers();

    /**
     * @brief Cleans up resources for a client socket
     * 
     * @param fd The client socket file descriptor
     * @param epoll_fd The epoll file descriptor
     */
    void cleanupClientSocket(int fd, int epoll_fd);

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

	/**
	* @brief Static callback function for logging new connections
	* 
	* @param fd The file descriptor of the new connection
	* @param data Pointer to the ClientSocket instance
	*/
	static void logNewConnection(int fd, void* data);

    /**
     * @brief Adds a callback to be executed immediately
     * 
     * @param callback The callback to execute
     */
    void executeImmediate(Callback* callback);

    /**
     * @brief Adds a callback to be executed later
     * 
     * @param callback The callback to execute
     * @param priority The priority of the callback
     */
    void executeDeferred(Callback* callback, CallbackManager::Priority priority = CallbackManager::NORMAL);

    /**
     * @brief Processes all deferred callbacks
     * 
     * Executes all deferred callbacks in priority order.
     */
    void processDeferredCallbacks();

    /**
     * @brief Cancels all callbacks for a specific file descriptor
     * 
     * @param fd The file descriptor to cancel callbacks for
     * @return int Number of callbacks cancelled
     */
    int cancelCallbacksForFd(int fd);


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

    
private:

    std::map<int, ServerSocket*>	_serverSockets;  // Map of server sockets by file descriptor
    std::map<int, ServerConfig>		_serverConfigs;  // Map of server configurations by server socket fd
    std::map<int, ClientSocket*>	_clientSockets;  ///< Map of client sockets by file descriptor
    int								_serverSocketFd; ///< Server socket file descriptor
    int								_clientSocketFd; ///< Client socket file descriptor (most recent)
    std::priority_queue<Timer>		_timerQueue;      ///< Priority queue of timers
    CallbackManager					_callbackManager;  ///< Callback manager
	
    // Helper method to register a server socket with epoll
    void registerServerSocketToEpoll(int epoll_fd, int server_socket_fd);
};

#endif  // ************************************************ SOCKETMANAGER_HPP //

