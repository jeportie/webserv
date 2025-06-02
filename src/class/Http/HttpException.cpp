/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:17:02 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/02 17:47:54 by jeportie         ###   ########.fr       */
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


void sendErrorResponse(int fd, int status, const std::string& reason)
{
    std::string statusText = reason;
    std::string bodyText   = reason;

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

    std::string res = oss.str();  // <-- ici copie par valeur
    write(fd, res.c_str(), res.size());
}


void sendCustomErrorResponse(int fd, int status, const std::string& errorPagePath)
{
    std::string statusMessage = getStatusMessage(status);
    std::string body;
    bool        fileLoaded = readFileContent(errorPagePath, body);

    if (!fileLoaded)
    {
        // Fallback : page HTML simple générée
        std::ostringstream fallback;
        fallback << "<html><head><title>" << status << " " << statusMessage << "</title></head>\n"
                 << "<body><h1>" << status << " " << statusMessage << "</h1>\n"
                 << "<p>The server encountered an error.</p></body></html>";
        body = fallback.str();
    }

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << statusMessage << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << body;

    std::string response = oss.str();
    write(fd, response.c_str(), response.size());
}
