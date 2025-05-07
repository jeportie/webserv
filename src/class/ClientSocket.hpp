/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 13:21:36 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "Socket.hpp"
# include <netinet/in.h>
# include <sys/socket.h>
# include <string>

class ClientSocket : public Socket
{
public:
	ClientSocket(void);
    ClientSocket(ClientSocket&&)            = delete;
    ClientSocket& operator=(ClientSocket&&) = delete;
    virtual ~ClientSocket(void);

	std::string	getClient(void) const;
	int			getClientPort() const;

private:
	ClientSocket(const ClientSocket& src);
	ClientSocket& operator=(const ClientSocket& rhs);

	struct sockaddr_in _clientAddr;
	socklen_t _clientAddrLen;
};


#endif  // ************************************************* CLIENTSOCKET_HPP //
