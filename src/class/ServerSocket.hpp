/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:39 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 22:43:17 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include "Socket.hpp"
# include "ClientSocket.hpp"
# include <netinet/in.h>
# include <string>
# include <cstring>

/**
 * @brief Server socket class that handles listening for connections
 * 
 * This class extends the Socket base class to provide functionality
 * specific to server sockets. It handles binding to a port, listening
 * for connections, and accepting new client connections.
 */
class ServerSocket: public Socket
{
public:
	ServerSocket(void);
	virtual ~ServerSocket(void);

    /**
     * Implementations of pure virtual method from Socket base class
     */
    virtual int setNonBlocking(int fd);
	virtual int safeFcntl(int fd, int cmd, int flag);

    /**
     * @brief Binds the socket to a specific port and address
     * 
     * @param port The port number to bind to
     * @param address The IP address to bind to (default: all interfaces)
     * @return true if binding was successful, false otherwise
     */
	bool safeBind(int port, const std::string& address);
    
    /**
     * @brief Starts listening for connections
     * 
     * @param backlog Maximum length of the queue of pending connections
     */
	void safeListen(int backlog);
    
    /**
     * @brief Accepts a new client connection
     * 
     * @param epoll_fd The epoll file descriptor to register the client with
     * @return ClientSocket* Pointer to a new ClientSocket object
     */
	ClientSocket* safeAccept(int epoll_fd);
    
    /**
     * @brief Gets the port number this socket is bound to
     * 
     * @return int The port number
     */
	int getPort(void) const;
    
    /**
     * @brief Gets the IP address this socket is bound to
     * 
     * @return std::string The IP address
     */
	std::string getAddress(void) const;

private:
    /**
     * @brief Copy constructor (private to prevent copying)
     * 
     * @param src The source object to copy from
     */
	ServerSocket(const ServerSocket& src);
    
    /**
     * @brief Assignment operator (private to prevent assignment)
     * 
     * @param rhs The right-hand side object to assign from
     * @return ServerSocket& Reference to this object
     */
	ServerSocket& operator=(const ServerSocket& rhs);

    struct sockaddr_in _serverAddr;  ///< Server address structure
};

#endif  // ************************************************* SERVERSOCKET_HPP //
