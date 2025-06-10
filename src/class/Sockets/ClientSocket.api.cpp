/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.api.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:11:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/10 17:27:31 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

void ClientSocket::touch()
{
    std::string clientID;

    clientID = _socketFd;
    LOG_SYSTEM_ERROR(DEBUG, SOCKET_ERROR, LOG_UPDATED_TIMESTAMP + clientID, __FUNCTION__);
    _lastActivity = time(NULL);
}

int ClientSocket::safeFcntl(int fd, int cmd, int flag)
{
    int               ret;
    std::stringstream ss;

    ret = fcntl(fd, cmd, flag);
    if (ret == -1)
    {
        ss << fd;
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, LOG_FCNTL_FAILED + ss.str(), __FUNCTION__);
        return (-1);
    }
    return (ret);
}
