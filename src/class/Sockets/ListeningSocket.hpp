/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 14:50:15 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/27 15:12:08 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#define LOG_LISTENINGSOCKET_CONSTRUCTOR "ListeningSocket Constructor called."
#define LOG_LISTENINGSOCKET_DESTRUCTOR "ListeningSocket Destructor called."

#include "Socket.hpp"
#include "ClientSocket.hpp"

#include <netinet/in.h>
#include <string>
#include <cstring>

class ListeningSocket : public Socket
{
public:
    ListeningSocket(void);
    ListeningSocket(const ListeningSocket& src);
    virtual ~ListeningSocket(void);

	virtual int			setNonBlocking(int fd);
    virtual int			safeFcntl(int fd, int cmd, int flag);

    bool				safeBind(int port, const std::string& address);
    void				safeListen(int backlog);
    ClientSocket*		safeAccept(int epoll_fd);

    int					getPort(void) const;
    std::string			getAddress(void) const;

private:
    ListeningSocket&		operator=(const ListeningSocket& rhs);

    struct sockaddr_in	_ListeningAddr;  ///< Listening address structure
};

typedef std::vector<ListeningSocket> LSVECTOR;

#endif

