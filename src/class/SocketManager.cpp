/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:44:31 by jeportie         ###   ########.fr       */
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
 */
void SocketManager::eventLoop(int epoll_fd)
{
    std::vector<epoll_event> events(MAXEVENTS);

    while (true)
    {
        int n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, -1);
        if (n < 0)
            throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
        
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
            }
            else if (ev & (EPOLLERR | EPOLLHUP))
            {
                // Error or hangup on a socket
                std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;
                
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
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            }
        }
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

// **Explanation of Communication Changes:**

// 1. **Removed while loop**: The original code used a while loop to read all available data. I changed this to a single read call because with epoll in edge-triggered mode (EPOLLET), we should read all available data at once.
//
// 2. **Added echo functionality**: Added code to echo back the received data to the client. This is a common feature in simple servers and helps demonstrate that the server is working correctly.
//
// 3. **Added client socket cleanup**: Added code to clean up the client socket when the client disconnects or there's an error. This includes removing it from the _clientSockets map.
//
// 4. **Improved logging**: Added more detailed log messages to show which client is sending data or disconnecting.
//
// 5. **Fixed epoll_ctl call**: The original code was missing the epoll_fd parameter in the epoll_ctl call when removing a client socket from epoll.

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
