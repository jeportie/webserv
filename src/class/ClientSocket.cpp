/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 22:48:22 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../include/webserv.h"
# include "ClientSocket.hpp"
# include <cstring>
# include <iostream>
# include <sstream>
# include <cerrno>

/**
 * @brief Default constructor
 * 
 * Initializes the base Socket class and sets up the client address structure.
 * The address structure is zeroed out and the address length is set to the
 * size of the structure.
 */
ClientSocket::ClientSocket(void)
: Socket()
{
    std::memset(&_clientAddr, 0, sizeof(sockaddr_in));
    _clientAddrLen = sizeof(_clientAddr);
}

/**
 * @brief Destructor
 * 
 * Closes the socket if it's still open and releases any resources.
 * Calls the closeSocket method from the base Socket class.
 */
ClientSocket::~ClientSocket(void)
{
    closeSocket();
}

/**
 * @brief set socket flags 
 *
 * @param fd The file descriptor
 * @param cmd option flag
 * @param the flag option
 *
 * @return int Return value from fcntl call, -1 on error
 */
int ClientSocket::safeFcntl(int fd, int cmd, int flag)
{
    int ret = fcntl(fd, cmd, flag);
    if (ret == -1)
    {
        std::cerr << "[Error] fcntl failed on client fd " << fd
                  << ": " << strerror(errno) << std::endl;
        return -1;
    }
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
int ClientSocket::setNonBlocking(int fd)
{
    // Get current flags
    int flags = safeFcntl(fd, F_GETFL, 0);

    // Set new flags with O_NONBLOCK added
    int ret = safeFcntl(fd, F_SETFL, flags | O_NONBLOCK);

    // Update non-blocking flag
    _isNonBlocking = true;

    return (ret);
}

/**
 * @brief Gets the client's port number
 * 
 * Converts the port number from network byte order to host byte order
 * using a custom function to convert the IP address from network byte
 * order to a string.
 * 
 * @return int The client's port number
 */
int ClientSocket::getClientPort(void) const
{
    return (_clientAddr.sin_port);
}

/**
 * @brief Gets the client's IP address as a string
 * 
 * Converts the stored network address to a human-readable string
 * using the inet_ntop (network to presentation) function.
 * 
 * @return std::string The client's IP address
 */
std::string ClientSocket::getClientIP(void) const
{
    // Convert network byte order to host byte order
    uint32_t ip = ntohl(_clientAddr.sin_addr.s_addr);
    
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

/**
 * @brief Sets the client's address information
 * 
 * Used when accepting a new client connection to store the client's
 * address information for later use.
 * 
 * @param addr The client's address structure
 * @param addrLen The length of the address structure
 */
void ClientSocket::setClientAddr(const struct sockaddr_in& addr, socklen_t addrLen)
{
    _clientAddr = addr;
    _clientAddrLen = addrLen;
}

/**
 * @brief Gets the client's address structure
 * 
 * @return const struct sockaddr_in& Reference to the client's address structure
 */
const struct sockaddr_in& ClientSocket::getClientAddr(void) const
{
    return _clientAddr;
}

/**
 * @brief Gets the length of the client's address structure
 * 
 * @return socklen_t The length of the address structure
 */
socklen_t ClientSocket::getClientAddrLen(void) const
{
    return _clientAddrLen;
}
