/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.api.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:07:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/10 17:25:18 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Http/HttpException.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/epoll.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>

#include "../Http/RequestLine.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../Http/HttpParser.hpp"
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"
#include "../Http/StatusUtils.hpp"
#include "../Http/ContentGenerator.hpp"
#include "../SocketManager/SocketManager.hpp"
#include "ReadCallback.hpp"
#include "WriteCallback.hpp"
#include "ErrorCallback.hpp"
#include "CloseCallback.hpp"



bool ReadCallback::readFromClient(int fd, ClientSocket* client)
{
    ssize_t n;
    char tmp[4096];
    std::string& buf = client->requestData.getBuffer();

    while (true)
    {
        n = ::recv(fd, tmp, sizeof(tmp), 0); // preferable a read() dans le contexte de lecture sur socket. 

        if (n > 0)
        {
            buf.append(tmp, n);
            client->touch();
            continue;
        }
        else if (n == 0)
        {
            std::cout << "jai recu 0" << std::endl;
            _manager->getCallbackQueue().push(new CloseCallback(_fd, _manager, -1));
            return false; // signaler fermeture
        }
        else // n = -1
        {
            std::cout << "jai recu -1" << std::endl;

            if (buf.empty())
            {
                std::cout << "je suis empty" << std::endl;
                _manager->getCallbackQueue().push(new CloseCallback(_fd, _manager, -1));
                return false;
            }
            else
            {
                std::cout << "je suis pas empty" << std::endl;
                return true;
            }
        }
    }
    // Jamais atteint, mais pour être complet
    return true;
}


bool ReadCallback::parseClientHeaders(ClientSocket* client)
{
    if (client->requestData.getIsHeadersParsed())
        return true;

    std::string& buf     = client->requestData.getBuffer();
    size_t       hdr_end = buf.find("\r\n\r\n");
    if (hdr_end == std::string::npos)
        return false;

    std::string hdr_block = buf.substr(0, hdr_end);
    size_t      line_end  = hdr_block.find("\r\n");
    std::string firstLine = hdr_block.substr(0, line_end);
    
    RequestLine rl = HttpParser::parseRequestLine(firstLine);
    client->requestData.setRequestLine(rl);

    // Headers
    std::string                                     rest = hdr_block.substr(line_end + 2);
    std::map<std::string, std::vector<std::string> > hdrs = HttpParser::parseHeaders(rest);
    if (hdrs.size() > MAX_HEADER_COUNT)
        throw HttpException(431, "Request Header Fields Too Large", "");
    client->requestData.setParsedHeaders(hdrs);
    client->requestData.setHeadersParsed(true);
    buf.erase(0, hdr_end + 4);

    // Délégation au client pour détermination du mode de body
    client->requestData.determineBodyMode();
    return true;
}

static size_t hexToSize(const std::string& hex)
{
    size_t             result = 0;
    std::istringstream iss(hex);
    iss >> std::hex >> result;
    if (result == SIZE_MAX)
        throw HttpException(400, "Bad Request: Invalid chunk size format", "");
    if (result > MAX_CHUNK_SIZE)
        throw HttpException(413, "Payload Too Large", "");  // chunk trop gros
    return result;
}

bool ReadCallback::parseClientBody(ClientSocket* client)
{
    std::string& buf  = client->requestData.getBuffer();
    BodyMode     mode = client->requestData.getBodyMode();
    if (mode == BODY_CONTENT_LENGTH)
    {
        size_t needed = client->requestData.getContentLength();
        if (needed > MAX_BODY_SIZE)
            throw HttpException(413, "Payload Too Large", "");
        if (buf.size() < needed)
            return false;
        // leave body in buffer until buildHttpRequest
        return true;
    }
    else if (mode == BODY_CHUNKED)
    {
        // chunked state machine, append to internal temporary storage
        while (true)
        {

            if (client->requestData.getChunkSize() == 0)
            {
                size_t pos = buf.find("\r\n");
                if (pos == std::string::npos)
                    return false;
                size_t chunkLen = hexToSize(buf.substr(0, pos));
                if (buf.size() + chunkLen > MAX_BODY_SIZE)
                    throw HttpException(413, "Payload Too Large", "");
                client->requestData.setChunkSize(chunkLen);
                buf.erase(0, pos + 2);
                if (chunkLen == 0)
                {
                    size_t end = buf.find("\r\n");
                    if (end == std::string::npos)
                        return false;
                    buf.erase(0, end + 2);
                    break;
                }
            }
            size_t needed = client->requestData.getChunkSize();
            if (buf.size() < needed + 2)
                return false;
            client->requestData.getBodyAccumulator().append(buf.substr(0, needed));
            buf.erase(0, needed + 2);
            client->requestData.setChunkSize(0);
        }
        return true;
    }
    // No body
    return true;
}

HttpRequest ReadCallback::buildHttpRequest(ClientSocket* client)
{
    HttpRequest req;
    // fill body
    if (client->requestData.getBodyMode() == BODY_CONTENT_LENGTH)
    {
        req.body = client->requestData.getBuffer();
        client->requestData.getBuffer().erase(0, req.body.size());
    }
    else if (client->requestData.getBodyMode() == BODY_CHUNKED)
    {
        req.body = client->requestData.getBodyAccumulator();
        client->requestData.clearBodyAccumulator();
    }

    // fill request-line and headers
    RequestLine rl = client->requestData.getRequestLine();
    req.method     = rl.method;
    req.http_major = rl.http_major;
    req.http_minor = rl.http_minor;
    HttpParser::splitTarget(rl.target, req.path, req.raw_query);
    HttpParser::parsePathAndQuerry(req.path, req.raw_query);

    req.headers      = client->requestData.getParsedHeaders();
    req.query_params = HttpParser::parseQueryParams(req.raw_query);
    req.form_data    = HttpParser::parseFormUrlencoded(req.body);
    return req;
}

void ReadCallback::cleanupRequest(ClientSocket* client) { client->requestData.resetParserState(); }

void ReadCallback::sendErrorResponse(int fd, int status, const std::string& message)
{
    std::ostringstream oss;
    std::string body;

    // Create a simple HTML error page
    std::ostringstream statusStream;
    statusStream << status;
    std::string statusStr = statusStream.str();
    body = "<html><head><title>Error " + statusStr + "</title></head>";
    body += "<body><h1>Error " + statusStr + "</h1>";
    body += "<p>" + message + "</p>";
    body += "</body></html>";

    // Build chunked headers
    oss << "HTTP/1.1 " << status << " " << message << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Transfer-Encoding: chunked\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    std::string header = oss.str();

    // Queue a WriteCallback to send the error response (body chunked)
    _manager->getWaitingList()[fd].getQueue().push(
        new WriteCallback(fd, _manager, header, body, _epoll_fd));
}

void ReadCallback::sendCustomErrorResponse(int fd, int status, const std::string& customPage)
{
    std::string statusMessage = getStatusMessage(status);
    std::string body;
    bool fileLoaded = readFileContent(customPage, body);

    if (!fileLoaded)
    {
        std::ostringstream fallback;
        fallback << "<html><head><title>" << status << " " << statusMessage << "</title></head>\n"
                 << "<body><h1>" << status << " " << statusMessage << "</h1>\n"
                 << "<p>The server encountered an error.</p></body></html>";
        body = fallback.str();
    }

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << statusMessage << "\r\n";
    oss << "Content-Type: text/html\r\n";
    oss << "Transfer-Encoding: chunked\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    std::string header = oss.str();

    // Queue a WriteCallback to send the custom error response (body chunked)
    _manager->getWaitingList()[fd].getQueue().push(
        new WriteCallback(fd, _manager, header, body, _epoll_fd));
}


