/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:17:24 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/15 17:40:26 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "RequestLine.hpp"
#include <cctype>    // isspace, isdigit
#include <sstream>   // pour stringstream si besoin
#include <cerrno>    // errno
#include <iostream>
#include <cstring>   // strerror


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

// 3) parseHeaders. It can acceot multiple headers with the same name such as cookies etc.
std::map<std::string,std::vector<std::string> > HttpParser::parseHeaders(const std::string& hdr_block) {
  std::map<std::string,std::vector<std::string> > headers;
  size_t start = 0, end;
  while ((end = hdr_block.find("\r\n", start)) != std::string::npos) {
    std::string line = hdr_block.substr(start, end - start);
    if (line.empty()) break; // fin des en-têtes
    size_t colon = line.find(':');
    if (colon != std::string::npos) {
      std::string name  = trim(line.substr(0, colon));
      std::string value = trim(line.substr(colon + 1));

      // normaliser case-insensitive : ici on laisse tel quel ou à toi de transformer en lowercase
      if(name.empty() == 0 && value.empty() == 0)
        headers[name].push_back(value);
    }
    start = end + 2;
  }
  return headers;
}

// // 4) readFixedBody
// std::string HttpParser::readFixedBody(int sockfd, size_t length) {
//   std::string body;
//   body.reserve(length);
//   char buf[1024];
//   size_t total = 0;
//   while (total < length) {
//     ssize_t r = read(sockfd, buf, std::min(sizeof(buf), length - total));
//     if (r <= 0 || r == 0) break; // client fermé ou erreur, ou r == 0 --> plus rien a lire (non bloquant)
//     body.append(buf, r);
//     total += r;
//   }
//   return body;
// }


// 4) splitTarget
void HttpParser::splitTarget(const std::string& target, std::string& outPath, std::string& outRawQuery)
{
    size_t pos = target.find('?');
    if (pos == std::string::npos) {
        outPath     = target;
        outRawQuery.clear();
    } else {
        outPath     = target.substr(0, pos);
        outRawQuery = target.substr(pos+1);
    }
}

// 5) parseQueryParams
std::map<std::string,std::string>
HttpParser::parseQueryParams(const std::string& raw_query)
{
    std::map<std::string,std::string> params;
    size_t start = 0, end;
    while ((end = raw_query.find('&', start)) != std::string::npos) {
        std::string token = raw_query.substr(start, end - start);
        std::string key, val;
        splitKeyVal(token, key, val);
        params[key] = val;
        start = end + 1;
    }
    if (start < raw_query.size()) {
        std::string token = raw_query.substr(start);
        std::string key, val;
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

std::map<std::string,std::string>
HttpParser::parseFormUrlencoded(const std::string& body)
{
    return parseQueryParams(body);
}


