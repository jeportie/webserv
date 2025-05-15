/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:17:22 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/15 17:42:38 by anastruc         ###   ########.fr       */
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







#include <string>
#include <map>
#include <vector>

#include "HttpRequest.hpp"   // contient enum Method, struct RequestLine, struct HttpRequest


// === Parser HTTP (tout static, pas d’état interne) ===
class HttpParser
{
public:
    // 1) Méthode HTTP
    static HttpRequest::Method
    parseMethod(const std::string& token);

    // 2) Request-Line complète
    static RequestLine
    parseRequestLine(const std::string& line);

    // 3) Toutes les lignes d’en-têtes (avant "\r\n\r\n")
    //    map<nom, liste de valeurs>
    static std::map<std::string,std::vector<std::string> >
    parseHeaders(const std::string& hdr_block);

    // 4) Sépare target en path + raw_query
    static void
    splitTarget(const std::string& target,
                std::string&      outPath,
                std::string&      outRawQuery);

    // 5) Paramètres de requête (query string) : map<clé, valeur>
    static std::map<std::string,std::string>
    parseQueryParams(const std::string& raw_query);

    // 6) Form data x-www-form-urlencoded : map<clé, valeur>
    static std::map<std::string,std::string>
    parseFormUrlencoded(const std::string& body);
};

std::string urlDecode(const std::string &s);
void	splitKeyVal(const std::string &token, std::string &key,
		std::string &val);
std::string trim(const std::string &s);
        


#endif // HTTPPARSER_HPP


// L'utilisation des Static ici est motive par le concept de realiser une boite a outils.
// Un parser n'a pas besoin d'avoir une instance. C'est simplement une "usine" 
// dans laquelle passe un input ici : ce qui est dans le buffer de chaque socket de communication client. 
// et recrache un output qu'il store dans la structure HttpRequest. 
