/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:17:22 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/14 16:06:07 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// HttpParser.hpp
#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>  // pour read()
#include "HttpRequest.hpp"
#include "RequestLine.hpp"



// === Parser HTTP (tout static, pas d’état interne) ===
class HttpParser {
public:

  // 1) Convertit un token ("GET", "POST", ...) en HttpRequest::Method
  static HttpRequest::Method parseMethod(const std::string& token);

  // 2) Découpe "METHOD target HTTP/x.y" en RequestLine
  static RequestLine parseRequestLine(const std::string& line);

  // 3) Parse un bloc d’en-têtes (séparé par "\r\n\r\n") en map<nom,valeur>
  static std::map<std::string,std::vector<std::string> >
  parseHeaders(const std::string& hdr_block);

  // 4) Lit exactement ‘length’ octets du socket (Content-Length)
  static std::string readFixedBody(int sockfd, size_t length);

  // à venir : chunked, query params, form data, security...
};

#endif 

// L'utilisation des Static ici est motive par le concept de realiser une boite a outils.
// Un parser n'a pas besoin d'avoir une instance. C'est simplement une "usine" 
// dans laquelle passe un input ici : ce qui est dans le buffer de chaque socket de communication client. 
// et recrache un output qu'il store dans la structure HttpRequest. 
