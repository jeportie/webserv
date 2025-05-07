/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:12:59 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 12:44:03 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

class Socket
{
public:
	Socket(void);
	virtual ~Socket(void);

	bool		socketCreate(void);
	virtual int setNonBlocking(int fd) = 0;

	// to implement:
	bool		setReuseAddr(bool reuse); // init reuse to true.
	
	int			getFd(void)			const;
	bool		isValid(void)		const;	
	bool		isNonBlocking(void) const; // to dig
	
    void		closeSocket(void);

protected:
	Socket(const Socket& src);
	Socket& operator=(const Socket& rhs);

	int		_socketFd;
	bool	_isNonBlocking;
};


#endif  // ******************************************************* SOCKET_HPP //
