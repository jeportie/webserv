/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:52:12 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:02:41 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"


#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
: _statusCode(200)
, _statusMessage("OK")
, _body("")
{
}
HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int code, const std::string& message)
{
    _statusCode    = code;
    _statusMessage = message;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

void HttpResponse::setBody(const std::string& content) { _body = content; }

int HttpResponse::getStatusCode() const { return _statusCode; }

std::string HttpResponse::getStatusMessage() const { return _statusMessage; }

std::string HttpResponse::getHeader(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return "";
}

std::string HttpResponse::getBody() const { return _body; }

const std::map<std::string, std::string>& HttpResponse::getAllHeaders() const { return _headers; }
