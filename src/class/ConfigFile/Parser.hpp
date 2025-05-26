/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 15:32:24 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/26 11:43:51 by fsalomon         ###   ########.fr       */
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

# define SSCMAP std::map<std::string, std::vector<ServerConfig> >;
# define ROUTEMAP std::map<std::string, RouteConfig> 

struct ServerConfig;
struct RouteConfig;


class Parser
{
  public:
    Parser(Lexer& lexer);
    ~Parser();
    SSCMAP parseConfigFile();
    
    // mis en public pour les test unitaire mais possibilite de le mettre en private

      const Token& current() const;
      void advance();
      //server bloc directive  
        ServerConfig parseServerBlock();
        void parseListenDirective(std::string &host, int&port);
        std::vector<std::string> parseServerNameDirective();
        std::string parseRootDirective();
        std::vector<std::string> parseAllowedMethodsDirective();
        std::map<int, std::string> parseErrorPagesDirective();
        size_t parseClientMaxBodySizeDirective();
        ROUTEMAP parseLocationBlocks();
        
        //location bloc directive
        bool parseAutoindexDirective();
        std::string parseDefaultFileDirective();
        std::map<int, std::string> parseReturnDirective();
        std::pair<std::string, std::string> parseCgiExecutorsDirective();
        bool parseUploadEnabledDirective();
        std::string parseUploadStoreDirective();

        
    private :   
    Lexer& _lexer;
    Token _current;
};

#endif