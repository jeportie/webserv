/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 15:32:24 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/13 16:49:15 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include "Lexer.hpp"
#include <vector>

struct RouteConfig {
    std::string path; // le chemin de la route, ex: /images
    std::vector<std::string> allowedMethods;
    bool autoindex;
    std::string root;
    std::string defaultFile;
    std::map<std::string, std::string> cgiExecutors; // ex: ".php" => "/usr/bin/php-cgi"
    bool uploadEnabled;
    std::string uploadStore;
    int returnCode; // pour les redirections
    std::string returnUrl;

    RouteConfig();
};

struct ServerConfig {
    int port;
    std::string host;
    std::vector<std::string> serverNames;
    std::string root;
    std::vector<std::string> allowedMethods;
    bool autoindex;
    std::map<int, std::string>      error_pages;             // ex: {404: "/errors/404.html"}
    size_t                          client_max_body_size;    // en octets
    // Autres directives comme error_pages, cgi, etc.

    ServerConfig();
};


class Parser
{
  public:
    Parser(Lexer& lexer);
    ~Parser();
    ServerConfig parseServerBlock();

    private :   
    Lexer& _lexer;
    Token _currentToken;
    int parseListenDirective();
    std::vector<std::string> parseServerNameDirective();
    std::map<std::string, RouteConfig> parseLocationBlocks();
    std::string parseRootDirective();
    bool parseAutoindexDirective();
    const Token& current() const;
    void advance();
};

#endif