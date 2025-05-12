/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:14 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/12 19:22:19 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// HttpParser.cpp
#include "HttpParser.hpp"
#include <cctype>    // isspace, isdigit
#include <sstream>   // pour stringstream si besoin
#include <cerrno>    // errno
#include <cstring>   // strerror

// UTILITAIRES C++98
static std::string trim(const std::string& s) {
  size_t b = 0, e = s.size();
  while (b < e && isspace(s[b])) ++b;
  while (e > b && isspace(s[e-1])) --e;
  return s.substr(b, e - b);
}

// 1) parseMethod
HttpRequest::Method HttpParser::parseMethod(const std::string& token) {
  if (token == "GET")    return HttpRequest::METHOD_GET;
  if (token == "POST")   return HttpRequest::METHOD_POST;
  if (token == "PUT")    return HttpRequest::METHOD_PUT;
  if (token == "DELETE") return HttpRequest::METHOD_DELETE;
  return HttpRequest::METHOD_INVALID;
}

// 2) parseRequestLine
RequestLine HttpParser::parseRequestLine(const std::string& line) {
  RequestLine rl;
  // Séparer sur espaces
  std::vector<std::string> parts;
  size_t pos = 0, next;
  while ((next = line.find(' ', pos)) != std::string::npos) {
    parts.push_back(line.substr(pos, next - pos));
    pos = next + 1;
  }
  // Le dernier push car on a quitte la boucle car il n 'y a plus d'espace mais on veut recuperer tout
  // ce qu'il y a apres le dernier espace. 
  parts.push_back(line.substr(pos));
  

  if (parts.size() != 3) {
    rl.method = HttpRequest::METHOD_INVALID;
    return rl;
  }

  rl.method = parseMethod(parts[0]);
  rl.target = parts[1];

  // Version HTTP/x.y
  if (parts[2].compare(0, 5, "HTTP/") == 0 && parts[2].size() >= 7) {
    char maj = parts[2][5], min = parts[2][7];
    rl.http_major = isdigit(maj) ? maj - '0' : 0;
    rl.http_minor = isdigit(min) ? min - '0' : 0;
  } else {
    rl.http_major = rl.http_minor = 0;
  }

  return rl;
}

// 3) parseHeaders
std::map<std::string, std::string>
HttpParser::parseHeaders(const std::string& hdr_block) {
  std::map<std::string, std::string> headers;
  size_t start = 0, end;
  while ((end = hdr_block.find("\r\n", start)) != std::string::npos) {
    std::string line = hdr_block.substr(start, end - start);
    if (line.empty()) break; // fin des en-têtes
    size_t colon = line.find(':');
    if (colon != std::string::npos) {
      std::string name  = trim(line.substr(0, colon));
      std::string value = trim(line.substr(colon + 1));
      // normaliser case-insensitive : ici on laisse tel quel ou à toi de transformer en lowercase
      headers[name] = value;
    }
    start = end + 2;
  }
  return headers;
}

// 4) readFixedBody
std::string HttpParser::readFixedBody(int sockfd, size_t length) {
  std::string body;
  body.reserve(length);
  char buf[1024];
  size_t total = 0;
  while (total < length) {
    ssize_t r = read(sockfd, buf, std::min(sizeof(buf), length - total));
    if (r <= 0) break; // client fermé ou erreur (non bloquant)
    body.append(buf, r);
    total += r;
  }
  return body;
}
