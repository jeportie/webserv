/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:53:50 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/06 17:24:02 by anastruc         ###   ########.fr       */
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

	void	init_connect(void);			
	void	communication(int fd);			

	int		getServerSocket(void) const;
	int		getClientSocket(void) const;

	bool	checkServerSocketAdresse(void) const;
	bool	safeBind(void);
	bool	safeListen(void);
	bool	safeAccept(int epoll_fd);
	void	eventLoop(int epoll_fd);


private:
	Server(const Server& src);
	Server&	operator=(const Server& rhs);

	int					_serverSocketFd;
	struct sockaddr_in	_serverSocketAdresse;

	int					_clientSocketFd;
	struct sockaddr_in	_clientSocketAdresse;
	socklen_t			_clientSocketAdresseLength;

};

std::ostream & operator<<(std::ostream& out, const Server& in);

#endif  // ******************************************************* SERVER_HPP //
