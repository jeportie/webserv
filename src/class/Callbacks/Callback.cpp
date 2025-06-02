/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/02 17:51:09 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Callback.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

Callback::Callback(int fd)
: _fd(fd)
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, LOG_CALLB_CONST, __FUNCTION__);
}

Callback::~Callback()
{
	 LOG_ERROR(DEBUG, CALLBACK_ERROR, "Callback Destructor called.", __FUNCTION__);
}

int Callback::getFd() const
{
    return _fd;
}
