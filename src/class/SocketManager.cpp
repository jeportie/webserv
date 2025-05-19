/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 13:47:46 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <vector>
#include "SocketManager.hpp"
#include "Callback.hpp"
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"

SocketManager::SocketManager(void)
: _serverSocketFd(-1)
, _clientSocketFd(-1)
{}

SocketManager::~SocketManager(void)
{
    // Clean up server sockets
    for (std::map<int, ServerSocket*>::iterator it = _serverSockets.begin();
         it != _serverSockets.end(); ++it)
    {
        delete it->second;
    }
    _serverSockets.clear();
    
    // Clean up client sockets
    for (std::map<int, ClientSocket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end(); ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}

/**
 * @brief Initializes server connections based on configuration
 * 
 * Creates server sockets for each configured server, binds them to the specified
 * host:port combinations, and starts listening for connections. Then creates an
 * epoll instance and registers all server sockets with it.
 * 
 * @param configs Vector of server configurations
 */
void SocketManager::init_connect_with_config(const std::vector<ServerConfig>& configs)
{
    // Create a server socket for each configured server
    for (size_t i = 0; i < configs.size(); ++i)
    {
        const ServerConfig& config = configs[i];
        
        ServerSocket* serverSocket = new ServerSocket();
        if (!serverSocket->safeBind(config.port, config.host))
        {
            std::stringstream ss;
            ss << "Failed to bind server socket on " << config.host << ":" << config.port;
            LOG_ERROR(ERROR, SOCKET_ERROR, ss.str(), "SocketManager::init_connect_with_config");
            delete serverSocket;
            continue; // Try to bind other servers even if one fails
        }
        
        serverSocket->safeListen(10);
        int serverSocketFd = serverSocket->getFd();
        
        // Store the server socket and its configuration
        _serverSockets[serverSocketFd] = serverSocket;
        _serverConfigs[serverSocketFd] = config;
        
        std::cout << "Server listening on " << config.host << ":" << config.port << std::endl;
    }
    
    // If no servers were successfully bound, throw an error
    if (_serverSockets.empty())
    {
        THROW_ERROR(CRITICAL, SOCKET_ERROR, "Failed to bind any server sockets", 
                   "SocketManager::init_connect_with_config");
    }
    
    // Create epoll instance
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to create epoll instance", 
                          "SocketManager::init_connect_with_config");
    }
    
    // Register all server sockets with epoll
    for (std::map<int, ServerSocket*>::iterator it = _serverSockets.begin();
         it != _serverSockets.end(); ++it)
    {
        registerServerSocketToEpoll(epoll_fd, it->first);
    }
    
    // Start the event loop
    eventLoop(epoll_fd);
}

/**
 * @brief Runs the event loop
 * 
 * Waits for events on the registered file descriptors and handles them.
 *
 * Note: Added a try-catch block around the safeAccept call.
 * This is to catch any exceptions thrown by the ServerSocket's safeAccept 
 * method and log them, but continue the event loop. This is different from 
 * the main() error handling because we want the server to continue running 
 * even if one client connection fails.
 * 
 * @param epoll_fd The epoll file descriptor
 * @param timeout_ms Timeout in milliseconds, -1 for infinite
 */
void SocketManager::eventLoop(int epoll_fd, int timeout_ms)
{
    std::vector<epoll_event> events(MAXEVENTS);

    while (true)
    {
        // Process any expired timers
        processTimers();

        // Process any pending callbacks
        processDeferredCallbacks();
        
        // Calculate timeout for epoll_wait based on next timer expiration
        int wait_timeout = timeout_ms;
        if (!_timerQueue.empty())
        {
            time_t now = time(NULL);
            time_t next_expire = _timerQueue.top().getExpireTime();
            if (next_expire <= now)
            {
                wait_timeout = 0; // Process immediately
            }
            else
            {
                wait_timeout = (next_expire - now) * 1000; // Convert to milliseconds
                if (timeout_ms != -1 && wait_timeout > timeout_ms)
                {
                    wait_timeout = timeout_ms; // Use the smaller timeout
                }
            }
        }

        // If there are pending callbacks, don't wait too long
        if (_callbackManager.hasPendingCallbacks() && (wait_timeout == -1 || wait_timeout > 100))
        {
            wait_timeout = 100; // Check for callbacks at least every 100ms
        }

        int n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, wait_timeout);
        if (n < 0)
		{
            if (errno == EINTR)
                continue;
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", "SocketManager::eventLoop");
		}
        
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
    
            // Check if this is a server socket
            std::map<int, ServerSocket*>::iterator server_it = _serverSockets.find(fd);
            if ((ev & EPOLLIN) && server_it != _serverSockets.end())
            {
                // New connection on the server socket
                try
                {
                    // Get the server socket from the map
                    ServerSocket* serverSocket = _serverSockets[fd];
                    ClientSocket* client = serverSocket->safeAccept(epoll_fd);
                    if (client)
                    {
                        _clientSocketFd = client->getFd();
                        _clientSockets[_clientSocketFd] = client;
                        // Add a timeout for idle connections (60 seconds)
                        // We need to use a static function for C++98 compatibility
                        Callback* timeoutCallback = new Callback(&SocketManager::handleTimeout, _clientSocketFd, this, "timeout_handler");
                        addTimer(60, timeoutCallback);
                        // Schedule a callback to log the new connection
                        Callback* logCallback = new Callback(&SocketManager::logNewConnection, _clientSocketFd, client, "connection_logger");
                        executeDeferred(logCallback, CallbackManager::HIGH);
                    }
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR(ERROR, SOCKET_ERROR, "Accept failed: " + std::string(e.what()), "SocketManager::eventLoop");
                }
            }
            else if ((ev & EPOLLIN) && fd != _serverSocketFd)
            {
                // Data available on a client socket
                communication(fd);
                // Reset the timeout for this client
                cancelTimer(fd);
                Callback* timeoutCallback = new Callback(&SocketManager::handleTimeout, fd, this);
                addTimer(60, timeoutCallback);
            }
            else if (ev & (EPOLLERR | EPOLLHUP))
            {
                // Error or hangup on a socket
                std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;
                cleanupClientSocket(fd, epoll_fd);
            }
        }
    }
}

/**
 * @brief Static callback function for logging new connections
 * 
 * @param fd The file descriptor of the new connection
 * @param data Pointer to the ClientSocket instance
 */
void SocketManager::logNewConnection(int fd, void* data)
{
    ClientSocket* client = static_cast<ClientSocket*>(data);
    std::cout << "New connection from " << client->getClientIP() 
              << ":" << client->getClientPort() 
              << " (fd=" << fd << ")" << std::endl;
}

/**
 * @brief Adds a callback to be executed immediately
 * 
 * @param callback The callback to execute
 */
void SocketManager::executeImmediate(Callback* callback)
{
    _callbackManager.executeImmediate(callback);
}

/**
 * @brief Adds a callback to be executed later
 * 
 * @param callback The callback to execute
 * @param priority The priority of the callback
 */
void SocketManager::executeDeferred(Callback* callback, CallbackManager::Priority priority)
{
    _callbackManager.executeDeferred(callback, priority);
}

/**
 * @brief Processes all deferred callbacks
 * 
 * Executes all deferred callbacks in priority order.
 */
void SocketManager::processDeferredCallbacks()
{
    _callbackManager.processDeferredCallbacks();
}

/**
 * @brief Cancels all callbacks for a specific file descriptor
 * 
 * @param fd The file descriptor to cancel callbacks for
 * @return int Number of callbacks cancelled
 */
int SocketManager::cancelCallbacksForFd(int fd)
{
    return _callbackManager.cancelCallbacksForFd(fd);
}

/**
 * @brief Cleans up resources for a client socket
 * 
 * @param fd The client socket file descriptor
 * @param epoll_fd The epoll file descriptor
 */
void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    // Cancel any timers for this client
    cancelTimer(fd);
    
    // Cancel any callbacks for this client
    cancelCallbacksForFd(fd);
    
    // Clean up the client socket
    if (fd != _serverSocketFd)
    {
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        // Remove from epoll
        if (epoll_fd >= 0)
        {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        }
        close(fd);
    }
}

/**
 * @brief Static callback function for handling timeouts
 * 
 * @param fd The file descriptor that timed out
 * @param data Pointer to the SocketManager instance
 */
void SocketManager::handleTimeout(int fd, void* data)
{
    SocketManager* manager = static_cast<SocketManager*>(data);
    std::cout << "Client connection timed out (fd=" << fd << ")" << std::endl;
    
    // Find the epoll_fd from the manager
    // This is a bit of a hack, but we need to get the epoll_fd somehow
    int epoll_fd = -1; // In a real implementation, you'd store this in the manager
    
    manager->cleanupClientSocket(fd, epoll_fd);
}

/**
 * @brief Adds a timer to the timer queue
 * 
 * @param seconds Seconds from now when the timer should expire
 * @param callback The callback to execute when the timer expires
 * @return int Timer ID
 */
int SocketManager::addTimer(int seconds, Callback* callback)
{
    time_t expireTime = time(NULL) + seconds;
    Timer timer(expireTime, callback);
    _timerQueue.push(timer);
    return callback->getFd(); // Use the file descriptor as the timer ID
}

/**
 * @brief Cancels a timer
 * 
 * @param fd The file descriptor associated with the timer
 * @return bool True if the timer was found and cancelled
 */
bool SocketManager::cancelTimer(int fd)
{
    // We can't easily remove from a priority queue, so we'll mark the callback as cancelled
    std::priority_queue<Timer> temp;
    bool found = false;
    
    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();
        _timerQueue.pop();
        
        Callback* callback = timer.getCallback();
        if (callback->getFd() == fd)
        {
            callback->cancel();
            found = true;
        }
        
        temp.push(timer);
    }
    
    _timerQueue = temp;
	return (found);
}

/**
 * @brief Processes expired timers
 * 
 * Checks for expired timers and executes their callbacks.
 */
void SocketManager::processTimers()
{
    time_t now = time(NULL);
    
    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();
        
        if (timer.getExpireTime() > now)
        {
            // No more expired timers
            break;
        }
        
        // Remove the timer from the queue
        _timerQueue.pop();
        
        // Execute the callback if it's not cancelled
        Callback* callback = timer.getCallback();
        if (!callback->isCancelled())
        {
            callback->execute();
        }
        // Callback is owned by the Timer, which will be destroyed when it goes out of scope
    }
}

/**
 * @brief Handles communication with a client
 * 
 * Reads data from a client socket and processes it.
 * 
 * @param fd The client socket file descriptor
 */
void SocketManager::communication(int fd)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        std::cout << "Received from client (fd=" << fd << "): " << buffer;
        
        // Echo back to the client
        if (write(fd, buffer, bytes_read) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, SOCKET_ERROR, "Failed to write response to client", "SocketManager::communication");
        }
    }
    else if (bytes_read == 0)
    {
        // Client closed the connection
        std::stringstream ss;
        ss << fd;
        LOG_ERROR(INFO, SOCKET_ERROR, "Client disconnected (fd=" + ss.str() + ")", "SocketManager::communication");
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        if (epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, EPOLL_ERROR, "Failed to remove client from epoll", "SocketManager::communication");
        }
        close(fd);
    }
    else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // Unexpected read error
        std::stringstream ss;
        ss << fd;
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, "Read error on fd " + ss.str(), "SocketManager::communication");
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        if (epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, EPOLL_ERROR, "Failed to remove client from epoll", "SocketManager::communication");
        }
        close(fd);
    }
    // If errno == EAGAIN or EWOULDBLOCK, we'll wait for the next EPOLLIN event
}

/**
 * @brief Sets the server socket to non-blocking mode
 * 
 * @param fd The file descriptor to set to non-blocking mode
 * @return int Return value from fcntl call, -1 on error
 */
int SocketManager::setNonBlockingServer(int fd)
{
    // Find the server socket in the map
    std::map<int, ServerSocket*>::iterator it = _serverSockets.find(fd);
    if (it != _serverSockets.end()) {
        return it->second->setNonBlocking(fd);
    }
    return -1;
}

/**
 * @brief Safely registers a file descriptor with epoll
 * 
 * @param epoll_fd The epoll file descriptor
 * @param op The operation to perform (EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL)
 * @param fd The file descriptor to register
 * @param event The epoll_event structure
 * @return int 0 on success, -1 on error
 */
int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        std::stringstream ss;
        ss << "epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd << ", op=" << op << ")";
        LOG_SYSTEM_ERROR(ERROR, EPOLL_ERROR, ss.str(),
                        "SocketManager::safeEpollCtlClient");
        return -1;
    }
    return 0;
}

/**
 * @brief Registers the server socket with epoll
 * 
 * @param epoll_fd The epoll file descriptor
 */
void SocketManager::safeRegisterToEpoll(int epoll_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _serverSocketFd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to add server socket to epoll", "SocketManager::safeRegisterToEpoll");
}

/**
 * @brief Registers a server socket with epoll
 * 
 * @param epoll_fd The epoll file descriptor
 * @param server_socket_fd The server socket file descriptor to register
 */
void SocketManager::registerServerSocketToEpoll(int epoll_fd, int server_socket_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_socket_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &ev) == -1)
    {
        std::stringstream ss;
        ss << "Failed to add server socket (fd=" << server_socket_fd << ") to epoll";
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, ss.str(), "SocketManager::registerServerSocketToEpoll");
    }
}


int SocketManager::getServerSocket(void) const { return _serverSocketFd; }

int SocketManager::getClientSocket(void) const { return _clientSocketFd; }
