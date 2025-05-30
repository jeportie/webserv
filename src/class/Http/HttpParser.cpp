/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:17:24 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/30 20:07:44 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpParser.hpp"
#include "HttpParser_utils.hpp"
#include "HttpRequest.hpp"
#include "HttpException.hpp"
#include "HttpLimits.hpp"
#include "RequestLine.hpp"

#include <cctype>   // isspace, isdigit
#include <cerrno>   // errno
#include <cstring>  // strerror

// 1) parseMethod
HttpRequest::Method HttpParser::parseMethod(const std::string& token)
{
    if (token == "GET")
        return HttpRequest::METHOD_GET;
    if (token == "POST")
        return HttpRequest::METHOD_POST;
    if (token == "DELETE")
        return HttpRequest::METHOD_DELETE;
    return HttpRequest::METHOD_INVALID;
}

// 2) parseRequestLine
RequestLine HttpParser::parseRequestLine(const std::string& line)
{
    RequestLine              rl;
    std::vector<std::string> parts;
    size_t                   pos;
    size_t                   next;
    char                     maj;
    char                     min;

    // Separate on spaces
    pos = 0;
    while (true)
    {
        next = line.find(' ', pos);
        if (next == std::string::npos)
            break;
        parts.push_back(line.substr(pos, next - pos));
        pos = next + 1;
    }
    // Le dernier push car on a quitte la boucle car il n 'y a plus d'espace mais on veut recuperer
    // tout ce qu'il y a apres le dernier espace.
    parts.push_back(line.substr(pos));

    if (parts.size() != 3)
        throw HttpException(400, "Bad Request: Invalid request line format", "");

    rl.method = parseMethod(parts[0]);
    if (rl.method == HttpRequest::METHOD_INVALID)
        throw HttpException(501, "Not Implemented", "");
    rl.target = parts[1];

    // Version HTTP/x.y
    if (parts[2].compare(0, 5, "HTTP/") == 0 && parts[2].size() >= 7)
    {
        maj           = parts[2][5];
        min           = parts[2][7];
        rl.http_major = isdigit(maj) ? maj - '0' : 0;
        rl.http_minor = isdigit(min) ? min - '0' : 0;
    }
    else
    {
        rl.http_major = 0;
        rl.http_minor = 0;
    }
    if (!(rl.http_major == 1 && (rl.http_minor == 0 || rl.http_minor == 1)))
        throw HttpException(505, "HTTP Version Not Supported", "");

    return rl;
}

SVSMAP HttpParser::parseHeaders(const std::string& hdr_block)
{
    SVSMAP      headers;
    size_t      start;
    size_t      end;
    std::string block;
    std::string line;
    size_t      colon;
    std::string name;
    std::string value;

    start = 0;
    block = hdr_block;
    if (block.size() < 2 || block.substr(block.size() - 2) != "\r\n")
        block += "\r\n";

    while (true)
    {
        end = block.find("\r\n", start);
        if (end == std::string::npos)
            break;
        line = block.substr(start, end - start);
        if (line.empty())
            break;

        colon = line.find(':');
        if (colon != std::string::npos)
        {
            name  = trim(line.substr(0, colon));
            value = trim(line.substr(colon + 1));
            if (name.length() > MAX_FIELD_NAME || value.length() > MAX_FIELD_VALUE)
                throw HttpException(431, "Request Header Fields Too Large", "");
            if (containsCtl(name) || containsCtl(value))
                throw HttpException(400, "Bad Request: Control characters in header name or value", "");
            if (!name.empty() && !value.empty())
                headers[name].push_back(value);
            else
                throw HttpException(400, "Bad Request: Empty header name or value", "");
        }
        else
            throw HttpException(400, "Bad Request: Missing colon in header line", "");

        start = end + 2;
    }
    return (headers);
}


// // 4) readFixedBody
// std::string HttpParser::readFixedBody(int sockfd, size_t length) {
//   std::string body;
//   body.reserve(length);
//   char buf[1024];
//   size_t total = 0;
//   while (total < length) {
//     ssize_t r = read(sockfd, buf, std::min(sizeof(buf), length - total));
//     if (r <= 0 || r == 0) break; // client fermé ou erreur, ou r == 0 --> plus rien a lire (non
//     bloquant) body.append(buf, r); total += r;
//   }
//   return body;
// }


// 4) splitTarget
void HttpParser::splitTarget(const std::string& target,
	std::string& outPath, std::string& outRawQuery)
{
    size_t pos = target.find('?');
    if (pos == std::string::npos)
    {
        outPath = target;
        outRawQuery.clear();
    }
    else
    {
        outPath     = target.substr(0, pos);
        outRawQuery = target.substr(pos + 1);
    }
}

void HttpParser::parsePathAndQuerry(std::string path, std::string raw_query)
{
    if (path.empty() || path[0] != '/')
        throw HttpException(400, "Bad Request: Invalid or empty path", "");

    if (path.size() > MAX_URI_LEN)
        throw HttpException(414, "URI Too Long: Path exceeds maximum allowed length", "");

    if (pathEscapesRoot(path))
        throw HttpException(403, "Forbidden: Path escapes server root directory", "");

    if (raw_query.size() > MAX_QUERY_LEN)
        throw HttpException(414, "URI Too Long: Query string exceeds maximum allowed length", "");
}


// 5) parseQueryParams
SSMAP HttpParser::parseQueryParams(const std::string& raw_query)
{
    SSMAP params;
    size_t start;
    size_t end;
    std::string token;
    std::string key;
    std::string val;

    start = 0;

    while (true)
    {
        end = raw_query.find('&', start);
        if (end == std::string::npos)
            break;
        token = raw_query.substr(start, end - start);
        splitKeyVal(token, key, val);
        params[key] = val;
        start = end + 1;
    }
    if (start < raw_query.size())
    {
        token = raw_query.substr(start);
        splitKeyVal(token, key, val);
        params[key] = val;
    }
    return params;
}

// parseFormUrlencoded :
// Dans le cas de l'envoi d'un formulaire HTTP en
// "application/x-www-form-urlencoded", le corps de la requête
// utilise exactement la même syntaxe que la query string :
// - une suite de paires clé=valeur séparées par '&'
// - les espaces sont encodés en '+'
// - les caractères spéciaux sont encodés en "%HH"
// Plutôt que de dupliquer la logique de découpage et de décodage,
// on réutilise donc directement le même parser (parseQueryParams),
// garantissant cohérence et maintenance simplifiée.

SSMAP HttpParser::parseFormUrlencoded(const std::string& body) { return parseQueryParams(body); }
