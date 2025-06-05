/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:17:02 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/05 11:07:25 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpException.hpp"
#include "ContentGenerator.hpp"
#include "StatusUtils.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

#include <sstream>
#include <string>
#include <unistd.h>

HttpException::HttpException(int status, const std::string& reason, const std::string& customPage)
: _status(status)
, _reason(reason)
, _customPage(customPage)
{
    LOG_ERROR(DEBUG, HTTP_REQ_ERROR, HTTPEXC_CONST, __FUNCTION__);
}

HttpException::~HttpException() throw()
{
    LOG_ERROR(DEBUG, HTTP_REQ_ERROR, HTTPEXC_DEST, __FUNCTION__);
}

int HttpException::status() const { return (_status); }

std::string HttpException::customPage() const { return (_customPage); }

const char* HttpException::what() const throw() { return (_reason.c_str()); }
