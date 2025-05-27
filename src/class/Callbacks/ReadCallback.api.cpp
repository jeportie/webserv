/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadCallback.api.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:07:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/27 16:50:42 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Http/HttpException.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/epoll.h>
#include <cerrno>
#include <vector>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>

#include "../Callbacks/ReadCallback.hpp"
#include "../Http/RequestLine.hpp"
#include "../Sockets/ClientSocket.hpp"
#include "../Http/HttpParser.hpp"
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"

bool ReadCallback::readFromClient(int fd, ClientSocket* client)
{
    ssize_t n;
    char tmp[4096];
    std::string& buf = client->requestData.getBuffer();

    while (true)
    {
        n = ::read(fd, tmp, sizeof(tmp));
        if (n > 0)
        {
            buf.append(tmp, n);
            client->touch();
            continue;
        }
        // n <= 0 : soit EOF (n==0), soit plus de données pour l'instant (n<0/EAGAIN)
        break;
    }
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

    // Request-Line
    std::cout << firstLine << std::endl;
    RequestLine rl = HttpParser::parseRequestLine(firstLine);
    client->requestData.setRequestLine(rl);

    // Headers
    std::string                                     rest = hdr_block.substr(line_end + 2);
    std::map<std::string, std::vector<std::string> > hdrs = HttpParser::parseHeaders(rest);
    if (hdrs.size() > MAX_HEADER_COUNT)
        throw HttpException(431, "Request Header Fields Too Large");
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
        throw HttpException(400, "Bad Request");  // taille invalide
    if (result > MAX_CHUNK_SIZE)
        throw HttpException(413, "Payload Too Large");  // chunk trop gros
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
            throw HttpException(413, "Payload Too Large");
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
                    throw HttpException(413, "Payload Too Large");
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
        req.body = client->requestData.getBuffer().substr(0, client->requestData.getContentLength());
        client->requestData.getBuffer().erase(0, client->requestData.getContentLength());
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


