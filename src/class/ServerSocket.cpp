/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:42:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 23:16:41 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Socket.hpp"
#include "ClientSocket.hpp"
#include <stdexcept>
#include <sys/epoll.h>
#include <iostream>

/**
 * @brief Default constructor
 * 
 * Initializes the base Socket class and sets up the server address structure.
 * The address structure is zeroed out.
 */
ServerSocket::ServerSocket(void)
: Socket()
{
    std::memset(&this->_serverAddr, 0, sizeof(sockaddr_in));
}

/**
 * @brief Destructor
 * 
 * Closes the socket if it's still open and releases any resources.
 * Calls the closeSocket method from the base Socket class.
 */
ServerSocket::~ServerSocket(void) { closeSocket(); }

/**
 * @brief set socket flags 
 *
 * @param fd The file descriptor
 * @param cmd option flag
 * @param the flag option
 *
 * @return int Return value from fcntl call, -1 on error
 */
int ServerSocket::safeFcntl(int fd, int cmd, int flag)
{
	int ret = fcntl(fd, cmd, flag);
	if (ret == -1)
		throw std::runtime_error("fcntl server socket failed");
	return ret;
}

/**
 * @brief Sets a file descriptor to non-blocking mode
 * 
 * Uses fcntl to get the current flags for the file descriptor,
 * adds the O_NONBLOCK flag, and sets the new flags.
 * Updates the _isNonBlocking flag if successful.
 * 
 * @param fd The file descriptor to set to non-blocking mode
 * @return int Return value from fcntl call, -1 on error
 */
int ServerSocket::setNonBlocking(int fd)
{
    int flags = safeFcntl(fd, F_GETFL, 0);
    int ret = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);

    _isNonBlocking = true;

    return (ret);
}

/**
 * @brief Binds the socket to a specific port and address
 * 
 * Creates a socket if one doesn't exist, sets socket options,
 * and binds the socket to the specified port and address.
 * 
 * @param port The port number to bind to
 * @param address The IP address to bind to (default: all interfaces)
 * @return true if binding was successful, false otherwise
 */
bool ServerSocket::safeBind(int port, const std::string& adress)
{
    // Create socket if it doesn't exist
    if (!isValid() && !socketCreate())
    {
        std::cerr << "[Error] Failed to create socket" << std::endl;
        return false;
    }

    setNonBlocking(this->_socketFd);
    _serverAddr.sin_port = htons(port);
    _serverAddr.sin_family      = AF_INET; 

    // Handle address (currently using INADDR_ANY)
    // In the future, this could be extended to use the address parameter
    _serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr*) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        std::cerr << "[Error] Bind failed: " << strerror(errno) << std::endl;
        return false;
    }
    return (true);
}

/**
 * @brief Starts listening for connections
 * 
 * @param backlog Maximum length of the queue of pending connections
 */
void ServerSocket::safeListen(int backlog)
{
    if (!isValid())
    {
        throw std::runtime_error("Cannot listen on invalid socket");
    }
    if (listen(this->_socketFd, backlog) < 0)
    {
        throw std::runtime_error("Listen() Failed: " + std::string(strerror(errno)));
    }
}

/**
 * @brief Accepts a new client connection
 * 
 * Creates a new ClientSocket object, accepts a connection from a client,
 * and sets the client socket's file descriptor and address information.
 * 
 * @param epoll_fd The epoll file descriptor to register the client with
 * @return ClientSocket* Pointer to a new ClientSocket object
 */
ClientSocket* ServerSocket::safeAccept(int epoll_fd)
{
    if (!isValid())
    {
        throw std::runtime_error("Cannot accept on invalid socket");
    }
    
    ClientSocket* client = new ClientSocket();
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    int clientFd = accept(this->_socketFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if (clientFd < 0)
    {
        delete client;
        throw std::runtime_error("Accept() Failed: " + std::string(strerror(errno)));
    }
    
    client->setFd(clientFd);
    client->setClientAddr(clientAddr, clientAddrLen);
    client->setNonBlocking(clientFd);
    
    std::cout << "Connection accepted from " << client->getClientIP() 
              << ":" << client->getClientPort() << std::endl;
    
    // If epoll_fd is valid, register the client with epoll
    if (epoll_fd >= 0)
    {
        epoll_event client_ev;
        client_ev.events = EPOLLIN | EPOLLET;
        client_ev.data.fd = clientFd;
        
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &client_ev) < 0)
        {
            delete client;
            throw std::runtime_error("Failed to add client to epoll: " + std::string(strerror(errno)));
        }
    }

    return (client);
}

/**
 * @brief Gets the port number this socket is bound to
 * 
 * @return int The port number
 */
int ServerSocket::getPort(void) const
{
    return ntohs(_serverAddr.sin_port);
}

/**
 * @brief Gets the IP address this socket is bound to
 * 
 * @return std::string The IP address
 */
std::string ServerSocket::getAddress(void) const
{
    // Convert network byte order to host byte order
    uint32_t ip = ntohl(_serverAddr.sin_addr.s_addr);
    
    // Extract each byte
    unsigned char bytes[4];
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;
    
    // Convert to string using C++ streams
    std::ostringstream oss;
    oss << static_cast<int>(bytes[0]) << "."
        << static_cast<int>(bytes[1]) << "."
        << static_cast<int>(bytes[2]) << "."
        << static_cast<int>(bytes[3]);
    
    return oss.str();
}
