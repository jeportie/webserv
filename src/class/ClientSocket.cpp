/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 14:29:39 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
# include <cstring>
# include <iostream>

ClientSocket::ClientSocket(void)
: Socket()
{
    std::memset(&_clientAddr, 0, sizeof(sockaddr_in));
    std::cout << "[ClientSocket] - default constructor called - " << std::endl;
}
ClientSocket::~ClientSocket(void)
{
    std::cout << "[Client] - destructor called - " << std::endl;
    closeSocket();
}

int ClientSocket::getClientPort(void) const
{
    return (_clientAddr.sin_port);
}

std::string ClientSocket::getClientIP(void) const
{
    // Convert network byte order to host byte order
    uint32_t ip = ntohl(_clientAddr.sin_addr.s_addr);
    
    // Extract each byte
    unsigned char bytes[4];
    bytes[0] =  ip >> 24;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] =  ip & 0xFF;
    
    // Convert to string
    char buffer[16]; // Max length of IPv4 address string (xxx.xxx.xxx.xxx\0)
    std::sprintf(buffer, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    
    return std::string(buffer);
}
