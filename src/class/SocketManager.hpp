/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 23:33:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <iostream>
# include <arpa/inet.h>

class SocketManager
{
public:
	SocketManager(void);
	~SocketManager(void);

    void init_connect(void);
    void communication(int fd);

    int getServerSocket(void) const;
    int setNonBlockingServer(int fd);
    int getClientSocket(void) const;
    
    void eventLoop(int epoll_fd);

private:
};

// Overload operator<< for output streaming
std::ostream & operator<<(std::ostream& out, const SocketManager& in);

#endif  // ************************************************ SOCKETMANAGER_HPP //
