/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:17:02 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/28 16:46:01 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpException.hpp"
#include "../../../include/webserv.h"

#include <sstream>
#include <string>
#include <unistd.h>

HttpException::HttpException(int status,
	const std::string &reason) : _status(status), _reason(reason)
{
	LOG_ERROR(DEBUG, HTTP_REQ_ERROR, HTTPEXC_CONST, __FUNCTION__);
}

HttpException::~HttpException() throw() 
{
	LOG_ERROR(DEBUG, HTTP_REQ_ERROR, HTTPEXC_DEST, __FUNCTION__);
}

int HttpException::status() const
{
	return (_status);
}
const char *HttpException::what() const throw()
{
	return (_reason.c_str());
}

void	sendErrorResponse(int fd, int status, const std::string &reason)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << status << " " << reason << "\r\n"
	<< "Content-Type: text/plain\r\n"
	<< "Content-Length: " << reason.size() << "\r\n"
	<< "Connection: close\r\n"
	<< "\r\n"
	<< reason;
	const std::string &res = oss.str();
	//REDIRECTION POSSIBLE SUR FICHIER STATIC ET URL
	write(fd, res.c_str(), res.size());
}
