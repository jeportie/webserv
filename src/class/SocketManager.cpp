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
    // Clean up client sockets
    for (std::map<int, ClientSocket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end(); ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}

/**
 * @brief Initializes the server connection
 * 
 * Creates a server socket, binds it to a port, and starts listening
 * for connections. Then creates an epoll instance and registers the
 * server socket with it.
 */
void SocketManager::init_connect(void)
{
    // Create, bind, and listen on the server socket
    if (!_serverSocket.safeBind(PORT, ""))
        throw std::runtime_error("Failed to bind server socket");
    
    _serverSocket.safeListen(10);
    _serverSocketFd = _serverSocket.getFd();
    
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
        throw std::runtime_error("Failed to create epoll instance: " + std::string(strerror(errno)));
    
    safeRegisterToEpoll(epoll_fd);
    std::cout << "Server listening on port " << PORT << std::endl;
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
            throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
		}
        
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
    
            if ((ev & EPOLLIN) && fd == _serverSocketFd)
            {
                // New connection on the server socket
                try
                {
                    ClientSocket* client = _serverSocket.safeAccept(epoll_fd);
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
                    std::cerr << "Accept failed: " << e.what() << std::endl;
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
        write(fd, buffer, bytes_read);
    }
    else if (bytes_read == 0)
    {
        // Client closed the connection
        std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }
    else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // Unexpected read error
        std::cerr << "Read error on fd " << fd << ": " << strerror(errno) << std::endl;
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
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
    return _serverSocket.setNonBlocking(fd);
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
        std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd
                  << ", fd=" << fd << ", op=" << op << "): "
                  << strerror(errno) << std::endl;
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
        throw std::runtime_error("Failed to add server socket to epoll: " + std::string(strerror(errno)));
}

int SocketManager::getServerSocket(void) const { return _serverSocketFd; }

int SocketManager::getClientSocket(void) const { return _clientSocketFd; }
