/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:23:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/07 12:24:01 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

# include <iostream>

class SocketManager
{
public:
	SocketManager(void);
	SocketManager(const SocketManager& src);
	~SocketManager(void);

	SocketManager& operator=(const SocketManager& rhs);

private:
};

// Overload operator<< for output streaming
std::ostream & operator<<(std::ostream& out, const SocketManager& in);

#endif  // ************************************************ SOCKETMANAGER_HPP //
