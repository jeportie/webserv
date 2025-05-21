/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <ctime>

// Base class for all callbacks
class Callback
{
public:
    Callback(int fd);
    virtual ~Callback();

    virtual void execute() = 0;
    
    // Non-virtual method to get the file descriptor
    // This is safe to call even after derived class destruction
    int getFd() const;

protected:
    int _fd;  // File descriptor associated with this callback
};

#endif  // ************************************************** CALLBACK_HPP //
