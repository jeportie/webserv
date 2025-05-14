/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:03:58 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 14:12:17 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACK_HPP
# define CALLBACK_HPP

# include <cstddef>
#include <ctime>

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
	typedef void (*CallbackFunction) (int, void*); // typedef to function pointer to call

    /**
     * @brief Constructor
     * 
     * @param func The function to call
     * @param fd The file descriptor
     * @param data Additional data to pass to the function
     * @param source Source information for debugging (optional)
     */
    Callback(CallbackFunction func, int fd, void* data = NULL, const char* source = "unknown");
	~Callback(void);

	/**
	 * @brief Execute the stored callback function
	 *
	 * Invokes the callback function with its associated file descriptor and data,
	 * unless the callback has been cancelled.
	 */
	void	execute(void);

	/**
	 * @brief Cancel this callback
	 *
	 * Marks the callback as cancelled so that execute() will no longer invoke the function.
	 */
	void	cancel(void);

	/**
	 * @brief Get the file descriptor associated with this callback
	 *
	 * @return int The file descriptor or identifier
	 */
	int		getFd(void) const;

	/**
	 * @brief Check if the callback has been cancelled
	 *
	 * @return true If the callback is cancelled
	 * @return false If the callback is still active
	 */
	bool	isCancelled(void) const;

    /**
     * @brief Get the creation time of the callback
     * 
     * @return time_t Creation time
     */
    time_t getCreationTime() const;
    
    /**
     * @brief Get the source information for the callback
     * 
     * @return const char* Source information
     */
    const char* getSource() const;

private:
	CallbackFunction _function;		// the function to call
	int				 _fd;			// the file descriptor (socket)
	void*			 _data;			// extra data needed
	bool			 _cancelled;		// cancelled callback
    time_t			 _creationTime;  // When the callback was created
    const char*		 _source;		// Source information (e.g., function name)
};

#endif  // ***************************************************** CALLBACK_HPP //
