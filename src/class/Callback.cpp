/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Callback.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:39:49 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/13 14:47:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Callback.hpp"

Callback::Callback(CallbackFct func, int fd, void* data)
: _function(func)
, _fd(fd)
, _data(data)
, _cancelled(false)
{}

Callback::~Callback() {}

void Callback::execute()
{
    if (!this->_cancelled)
        this->_function(_fd, _data);
}

void Callback::cancel() { _cancelled = true; }

int Callback::getFd() const { return _fd; }

bool Callback::isCancelled() const { return _cancelled; }
