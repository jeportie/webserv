/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:53:50 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 12:32:01 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <arpa/inet.h>


class Server
{
public:
    Server(void);  // init a -1 lesfd server et client
    ~Server(void);

    void init_connect(void);
    void communication(int fd);

    int getServerSocket(void) const;
    int setNonBlockingServer(int fd);
    int getClientSocket(void) const;
    
    void safeBind(void);
    void safeListen(void);
    void safeAccept(int epoll_fd);
    void safeRegisterToEpoll(int epoll_fd);

    void eventLoop(int epoll_fd);


private:
    Server(const Server& src);
    Server& operator=(const Server& rhs);

    int                _serverSocketFd;
    struct sockaddr_in _serverSockAdr;

    int                _clientSocketFd;
    struct sockaddr_in _clientSockAdr;
    socklen_t          _clientSockAdrLen;
};

std::ostream& operator<<(std::ostream& out, const Server& in);
int setNonBlockingClient(int fd);



#endif  // ******************************************************* SERVER_HPP //
