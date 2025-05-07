/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:12:59 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:15:08 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstring>

/**
 * @brief Abstract base class for socket operations
 * 
 * This class provides a common interface for socket operations,
 * with specific implementations provided by derived classes.
 * It encapsulates a socket file descriptor and provides methods
 * for creating, configuring, and managing sockets.
 */
class Socket
{
public:
    /**
     * @brief Default constructor
     * Initializes socket file descriptor to -1 (invalid)
     */
    Socket(void);
    
    /**
     * @brief Virtual destructor
     * Ensures proper cleanup of resources in derived classes
     */
    virtual ~Socket(void);

    /**
     * @brief Creates a socket using socket() system call
     * @return true if socket creation was successful, false otherwise
     */
    bool socketCreate(void);
    
    /**
     * @brief Pure virtual method to set socket to non-blocking mode
     * @param fd The file descriptor to set to non-blocking mode
     * @return int Return value from fcntl call, -1 on error
     */
    virtual int setNonBlocking(int fd) = 0;

    /**
     * @brief Pure virtual method to set socket flags 
     * @param fd The file descriptor
	 * @param cmd option flag
	 * @param the flag option
     * @return int Return value from fcntl call, -1 on error
     */
	virtual int safeFcntl(int fd, int cmd, int flag) = 0;
    
    /**
     * @brief Sets the SO_REUSEADDR socket option
     * 
     * This allows the socket to bind to an address that is already in use.
     * Useful for server sockets to:
     * - Restart quickly after a crash without waiting for socket timeout
     * - Allow multiple sockets to bind to the same address (different ports)
     * - Prevent "Address already in use" errors when restarting a server
     * 
     * @param reuse Whether to enable (true) or disable (false) the option
	 * @default-param true
     * @return true if successful, false otherwise
     */
    bool setReuseAddr(bool reuse = true);
    
    /**
     * @brief Gets the socket file descriptor
     * @return The socket file descriptor
     */
    int getFd(void) const;
    
    /**
     * @brief Sets the socket file descriptor
     * @param fd The new file descriptor
     */
    void setFd(int fd);
    
    /**
     * @brief Checks if the socket is valid (has a valid file descriptor)
     * @return true if valid, false otherwise
     */
    bool isValid(void) const;
    
    /**
     * @brief Checks if the socket is in non-blocking mode
     * @return true if non-blocking, false otherwise
     */
    bool isNonBlocking(void) const;
    
    /**
     * @brief Closes the socket
     */
    void closeSocket(void);

protected:
    /**
     * @brief Copy constructor (protected to prevent direct copying)
     * @param src The source object to copy from
     */
    Socket(const Socket& src);
    
    /**
     * @brief Assignment operator (protected to prevent direct assignment)
     * @param rhs The right-hand side object to assign from
     * @return Reference to this object
     */
    Socket& operator=(const Socket& rhs);

    int _socketFd;       ///< Socket file descriptor
    bool _isNonBlocking; ///< Flag indicating if socket is in non-blocking mode
};


#endif  // ******************************************************* SOCKET_HPP //
