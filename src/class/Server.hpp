/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:53:50 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/05 19:42:43 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <arpa/inet.h>

class Server
{
    public:
    Server(void);  // init a -1 lesfd server et client
    ~Server(void);
    Server(const Server& src);
    Server& operator=(const Server& rhs);
    
    void connect(void);
    void communication(void);

    int getServerSocket(void) const;
    int getClientSocket(void) const;

    int         getPort(void) const;
    std::string getServerName(void) const;
    std::string getRoot(void) const;
    std::string getIndex(void) const;
    std::string getErrorPage(void) const;
    
    
    void        setPort(int port);
    void        setServerName(std::string serverName);
    void        setRoot(std::string root);
    void        setIndex(std::string index);
    void        setErrorPage(std::string errorPage);

    bool checkServerSocketAdresse(void) const;
    bool safeBind(void);
    bool safeListen(void);
    bool safeAccept(void);


private:

    int                _serverSocketFd;
    struct sockaddr_in _serverSocketAdresse;

    int                _clientSocketFd;
    struct sockaddr_in _clientSocketAdresse;
    socklen_t          _clientSocketAdresseLength;

    int         _port;
    std::string _serverName;
    std::string _root;
    std::string _index;
    std::string _errorPage;
};

std::ostream& operator<<(std::ostream& out, const Server& in);

#endif  // ******************************************************* SERVER_HPP //
