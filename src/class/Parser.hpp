/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 15:32:24 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 15:13:56 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <vector>
# include <map>
# include <stdexcept>
# include <cstdlib>
# include <sstream>

# include "Lexer.hpp"
# include "ServerConfig.hpp"
# include "RouteConfig.hpp"

struct ServerConfig;
struct RouteConfig;


class Parser
{
  public:
    Parser(Lexer& lexer);
    ~Parser();
    ServerConfig parseServerBlock();
    
    
        void parseListenDirective(std::string &host, int&port);
        std::vector<std::string> parseServerNameDirective();
        std::string parseRootDirective();
        bool parseAutoindexDirective();
        std::vector<std::string> parseAllowedMethodsDirective();
        std::map<int, std::string> parseErrorPagesDirective();
        size_t parseClientMaxBodySizeDirective();
        std::string parseDefaultFileDirective();
        std::map<int, std::string> parseReturnDirective();
        std::pair<std::string, std::string> parseCgiExecutorsDirective();
        bool parseUploadEnabledDirective();
        std::string parseUploadStoreDirective();
        std::map<std::string, RouteConfig> parseLocationBlocks();
        const Token& current() const;
        void advance();
        
    private :   
    Lexer& _lexer;
    Token _current;
};

#endif