/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/20 10:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Callback.hpp"
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"

Callback::Callback(int fd)
: _fd(fd)
{
	// LOG_ERROR(DEBUG, CALLBACK_ERROR, "Callback Constructor called.",
	// 	"Callback.cpp->Callback(int fd)");
}

Callback::~Callback()
{
	// LOG_ERROR(DEBUG, CALLBACK_ERROR, "Callback Destructor called.",
	// 	   "Callback.cpp->~Callback()");
}

int Callback::getFd() const
{
    return _fd;
}
