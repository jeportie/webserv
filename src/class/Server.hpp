/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:53:50 by jeportie          #+#    #+#             */
/*   Updated: 2025/04/30 17:43:02 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <arpa/inet.h>

class Server
{
public:
	Server(void); // init a -1 lesfd server et client
	~Server(void);

	void	connect(void);			
	void	communication(void);			

	int		getServerSocket(void) const;
	int		getClientSocket(void) const;

	bool	checkServerSocketAdresse(void) const;
	bool	safeBind(void);
	bool	safeListen(void);
	bool	safeAccept(void);


private:
	Server(const Server& src);
	Server&	operator=(const Server& rhs);

	int					_serverSocketFd;
	struct sockaddr_in	_severSocketAdresse;

	int					_clientSocketFd;
	struct sockaddr_in	_clientSocketAdresse;
	size_t				_clientSocketAdresseLength;

};

std::ostream & operator<<(std::ostream& out, const Server& in);

#endif  // ******************************************************* SERVER_HPP //
