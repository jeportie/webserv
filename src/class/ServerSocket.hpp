/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:13:39 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 13:14:40 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include "Socket.hpp"
# include "ClientSocket.hpp"
#include <netinet/in.h>
# include <string>

class ServerSocket: public Socket
{
public:
	ServerSocket(void);
	virtual ~ServerSocket(void);

	bool			safeBind(int port, const std::string& adress);
	void			safeListen(int backlog); // SOMAXCONN
	ClientSocket*	safeAccept(int epoll_fd);

	int				getPort(void) const;
	std::string		getAdresse(void) const;

private:
	ServerSocket(const ServerSocket& src);
	ServerSocket& operator=(const ServerSocket& rhs);

	struct sockaddr_in _serverAddr;
};

#endif  // ************************************************* SERVERSOCKET_HPP //
