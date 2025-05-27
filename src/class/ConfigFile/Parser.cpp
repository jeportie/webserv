/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:21:43 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 12:02:11 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "ServerConfig.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"

Parser::Parser(Lexer& lexer)
: _lexer(lexer)
{
    _current = _lexer.nextToken();
}

Parser::~Parser(void) {}


const Token& Parser::current() const { return _current; }

void Parser::advance() { _current = _lexer.nextToken(); }


IVSCMAP Parser::parseConfigFile()
{
    IVSCMAP			serversByPort;
	ServerConfig	config;
	std::string		msg;

    while (current().type != TOKEN_EOF)
    {
        if (current().type == TOKEN_IDENTIFIER && current().value == "server")
        {
            config = parseServerBlock();
            // Valeur par défaut si aucun nom n’est spécifié
            if (config.serverNames.empty())
            {
                config.serverNames.push_back("default");
            }
            // Ajoute ce ServerConfig à la liste associée à ce :port
            serversByPort[config.port].push_back(config);
        }
        else
        {
			msg = "Only 'server' blocks are allowed at top level";
			THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, msg, __FUNCTION__);
        }
    }
    return (serversByPort);
}

ServerConfig Parser::parseServerBlock()
{
    ServerConfig config;
    std::string		error_msg;


    if (current().type != TOKEN_IDENTIFIER || current().value != "server")
    {
        error_msg = "Expected 'server'";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    if (current().type != TOKEN_LBRACE)
    {
        error_msg = "Expected '{' after 'server'";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    while (current().type != TOKEN_RBRACE && current().type != TOKEN_EOF)
    {
        if (current().type != TOKEN_IDENTIFIER)
        {
            error_msg = "Unexpected token in server block: " + current().value;
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
        if (current().value == "listen")
        {
            parseListenDirective(config.host, config.port);
            config.listenIsSet = true;
        }
        else if (current().value == "server_name")
        {
            config.serverNames = parseServerNameDirective();
        }
        else if (current().value == "root")
        {
            config.root      = parseRootDirective();
            config.rootIsSet = true;
        }
        else if (current().value == "allowed_methods")
        {
            config.allowedMethods = parseAllowedMethodsDirective();
        }
        else if (current().value == "error_page")
        {
            config.error_pages = parseErrorPagesDirective();
        }
        else if (current().value == "client_max_body_size")
        {
            config.client_max_body_size = parseClientMaxBodySizeDirective();
        }
        else if (current().value == "index")
        {
            config.indexFiles = parseIndexDirective();
            config.indexIsSet = true;
        }
        else if (current().value == "location")
        {
            config.routes = parseLocationBlocks();
        }
        else
        {
            error_msg = "Unknown directive in server block: " + current().value;
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
    }
    // Vérifie qu'on ferme bien le bloc avec '}'
    if (current().type != TOKEN_RBRACE)
    {
        error_msg = "Expected '}' to close server block";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    return config;
}

std::map<std::string, RouteConfig> Parser::parseLocationBlocks()
{
    std::map<std::string, RouteConfig> routes;
    std::string error_msg;
    std::string path;
    std::string directive;
    RouteConfig route;
    
    
    while (current().type == TOKEN_IDENTIFIER && current().value == "location")
    {
        advance();  // skip 'location'
        
        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        {
            error_msg = "Expected path after 'location'";
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }

        path = current().value;
        advance();

        if (current().type != TOKEN_LBRACE)
        {
            error_msg = "Expected '{' to begin location block";
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
        advance();
        route.path = path;

        while (current().type != TOKEN_RBRACE && current().type != TOKEN_EOF)
        {
            if (current().type != TOKEN_IDENTIFIER)
            {
                error_msg = "Expected directive inside location block";
		        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
            }
            directive = current().value;
            if (directive == "root")
            {
                route.root = parseRootDirective();
            }
            else if (directive == "autoindex")
            {
                route.autoindex = parseAutoindexDirective();
            }
            else if (directive == "allowed_methods")
            {
                route.allowedMethods = parseAllowedMethodsDirective();
            }
            else if (directive == "default_file")
            {
                route.defaultFile = parseDefaultFileDirective();
            }
            else if (directive == "return")
            {
                route.returnCodes = parseReturnDirective();
            }
            else if (directive == "cgi_executor")
            {
                route.cgiExecutor = parseCgiExecutorsDirective();
            }
            else if (directive == "upload_enable")
            {
                route.uploadEnabled = parseUploadEnabledDirective();
            }
            else if (directive == "upload_store")
            {
                route.uploadStore = parseUploadStoreDirective();
            }
                        else if (directive == "index")
            {
                route.indexFiles = parseIndexDirective();
                route.indexIsSet = true;
            }
            else if (directive == "client_max_body_size")
            {
                route.client_max_body_size = parseClientMaxBodySizeDirective();
            }
            else
            {
                error_msg = "Unknown directive in location block: " + directive;
		        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
            }
        }
        if (current().type != TOKEN_RBRACE)
        {
            error_msg = "Expected '}' to close location block";
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
        advance();
        routes.insert(std::make_pair(path, route));
    }
    return routes;
}
