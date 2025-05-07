/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:16:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 16:22:27 by fsalomon         ###   ########.fr       */
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
    virtual ~ClientSocket(void);

	std::string	getClientIP(void) const;
	int			getClientPort() const;

private:
	ClientSocket(const ClientSocket& src);
	ClientSocket& operator=(const ClientSocket& rhs);

	struct sockaddr_in _clientAddr;
	socklen_t _clientAddrLen;
};


#endif  // ************************************************* CLIENTSOCKET_HPP //
