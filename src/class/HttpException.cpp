/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:17:02 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/19 11:25:21 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpException.hpp"
#include <exception>
#include <sstream>
#include <string>
#include <unistd.h>

HttpException::HttpException(int status,
	const std::string &reason) : _status(status), _reason(reason)
{
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
	write(fd, res.c_str(), res.size());
}