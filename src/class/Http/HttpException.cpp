/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:17:02 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/30 20:00:52 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpException.hpp"
#include "../../../include/webserv.h"

#include <sstream>
#include <string>
#include <unistd.h>

HttpException::HttpException(int status,
	const std::string &reason, const std::string& customPage) : _status(status), _reason(reason), _customPage(customPage)
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

std::string HttpException::customPage() const
{
	return (_customPage);
}

const char *HttpException::what() const throw()
{
	return (_reason.c_str());
}


void sendErrorResponse(int fd, int status, const std::string& reason)
{
    std::string statusText = reason;
    std::string bodyText = reason;

    std::string::size_type colonPos = reason.find(':');
    if (colonPos != std::string::npos)
    {
        statusText = reason.substr(0, colonPos);

        std::string after = reason.substr(colonPos + 1);
		if (after.empty())
		{
    		bodyText = "";
		}
		 else 
		{
    		if (after[0] == ' ') 
			{
        		bodyText = after.substr(1);
    		}
			else 
			{
        		bodyText = after;
    		}
		}    
	}

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << statusText << "\r\n"
        << "Content-Type: text/plain\r\n"
        << "Content-Length: " << bodyText.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << bodyText;

    std::string res = oss.str(); // <-- ici copie par valeur
    write(fd, res.c_str(), res.size());
}


