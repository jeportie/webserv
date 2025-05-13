/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:03:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/13 15:15:03 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACK_HPP
# define CALLBACK_HPP

# include <cstddef>

/**
 * @brief Callback class for event handling
 * 
 * This class represents a callback function that can be executed
 * when a specific event occurs. It stores the function to call
 * and also any associated data.
 *
 * @note Callbacks and the typedef:
 *
 * Any free function (or static member function) matching that signature can 
 * be passed into the Callback constructor.
 * This lets your event loop or dispatcher be completely generic:
 * 	it doesn’t need to know what the callback actually does,
 * 	only that it takes an int (the file descriptor) and a void* (any user-supplied data).
 */
class Callback
{
public:
	typedef void (*CallbackFct) (int, void*); // typedef to function pointer to call

	Callback(CallbackFct func, int fd, void *data = NULL);
	~Callback(void);

	void	execute(void);
	void	cancel(void);
	int		getFd(void) const;
	bool	isCancelled(void) const;

private:
	CallbackFct _function;	// the function to call
	int			_fd;		// the file descriptor (socket)
	void*		_data;		// extra data needed
	bool		_cancelled;	// cancelled callback
};

#endif  // ***************************************************** CALLBACK_HPP //
