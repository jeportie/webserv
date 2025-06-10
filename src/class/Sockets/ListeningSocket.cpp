/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 14:51:58 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/10 17:27:44 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>

#include "ListeningSocket.hpp"
#include "Socket.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

ListeningSocket::ListeningSocket(void)
: Socket()
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_LISTENINGSOCKET_CONSTRUCTOR, __FUNCTION__);
    std::memset(&this->_ListeningAddr, 0, sizeof(sockaddr_in));
}

ListeningSocket::ListeningSocket(const ListeningSocket& src)
: Socket(src)
{
    std::memcpy(&this->_ListeningAddr, &src._ListeningAddr, sizeof(sockaddr_in));
}

ListeningSocket::~ListeningSocket(void)
{
    LOG_ERROR(DEBUG, SOCKET_ERROR, LOG_LISTENINGSOCKET_DESTRUCTOR, __FUNCTION__);
    // closeSocket();
}
