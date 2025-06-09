/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:52:12 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/09 15:01:51 by anastruc         ###   ########.fr       */
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
void HttpResponse::setFileToStream(const std::string& p) { _fileToStream = p; }

void HttpResponse::setBody(const std::string& content) { _body = content; }

const std::string& HttpResponse::getFileToStream() const { return _fileToStream; }

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

const std::map<std::string, std::string>& HttpResponse::getAllHeaders() const {
    return _headers;
}

#include <sstream>
#include <algorithm> // pour std::find_if, std::isspace

// Helper pour trim début et fin (espaces et \r)
static std::string trim(const std::string &s) {
    size_t start = 0, end = s.size();
    while (start < end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r')) ++start;
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) --end;
    return s.substr(start, end - start);
}

void HttpResponse::parseCgiOutputAndSet(const std::string& cgi_output)
{
    std::istringstream iss(cgi_output);
    std::string line;

    // 1. Headers
    while (std::getline(iss, line)) {
        if (line.empty() || line == "\r" || line == "\n" || line == "\r\n") {
            // Ligne vide = fin des headers
            break;
        }
        size_t sep = line.find(':');
        if (sep != std::string::npos) {
            std::string key = trim(line.substr(0, sep));
            std::string val = trim(line.substr(sep + 1));
            setHeader(key, val);
        }
    }

    // 2. Le reste = body (attention getline retire le \n)
    std::ostringstream body_ss;
    body_ss << iss.rdbuf();
    std::string body = body_ss.str();

    // Enlève éventuellement un \r\n ou \n initial du body
    if (!body.empty() && (body[0] == '\r' || body[0] == '\n'))
        body = body.substr(1);

    setBody(body);

    // Optionnel : sécurité, si le CGI n’a pas donné de Content-Type, on en met un
    if (!hasHeader("Content-Type"))
        setHeader("Content-Type", "text/html");
}

bool HttpResponse::hasHeader(const std::string& key) const
{
    return _headers.find(key) != _headers.end();
}

