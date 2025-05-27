/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 15:32:24 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 08:54:38 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>

#include "Lexer.hpp"

struct ServerConfig;
struct RouteConfig;

class Parser
{
public:
    Parser(Lexer& lexer);
    ~Parser();

    IVSCMAP			parseConfigFile();
    
    // mis en public pour les test unitaire mais possibilite de le mettre en private

	const Token&	current() const;
    void			advance();
    //server bloc directive  
    ServerConfig	parseServerBlock();
    void			parseListenDirective(std::string &host, int&port);
    SVECTOR			parseServerNameDirective();
    std::string		parseRootDirective();
    SVECTOR			parseAllowedMethodsDirective();
    ISMAP			parseErrorPagesDirective();
    size_t			parseClientMaxBodySizeDirective();
    ROUTEMAP		parseLocationBlocks();
    
    //location bloc directive
    bool			parseAutoindexDirective();
    std::string		parseDefaultFileDirective();
    ISMAP			parseReturnDirective();
    SSPAIR			parseCgiExecutorsDirective();
    bool			parseUploadEnabledDirective();
    std::string		parseUploadStoreDirective();

private :   
	Lexer&			_lexer;
	Token			_current;
};

#endif
