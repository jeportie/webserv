/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:21:43 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/13 18:25:50 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"


Parser::Parser(Lexer& lexer) : _lexer(lexer) {
    _current = _lexer.nextToken();
}

Parser::~Parser(void) {}

const Token& Parser::current() const {
    return _current;
}

void Parser::advance() {
    _current = _lexer.nextToken();
}

int Parser::parseListenDirective() {
    advance(); // skip 'listen'
    if (current().type != TOKEN_NUMBER)
        throw std::runtime_error("Expected port number after 'listen'");
    int port = std::atoi(current().value.c_str());

    advance();
    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after listen port");
    advance();
    return port;
}


std::vector<std::string> Parser::parseServerNameDirective() {
    advance(); // skip 'server_name'
    std::vector<std::string> names;

    while (current().type == TOKEN_IDENTIFIER || current().type == TOKEN_STRING) {
        names.push_back(current().value);
        advance();
    }

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after server_name values");
    advance();
    return names;
}


std::string Parser::parseRootDirective() {
    advance(); // skip 'root'
    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected path after 'root'");
    std::string root = current().value;

    advance();
    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after root");
    advance();
    return root;
}


bool Parser::parseAutoindexDirective() {
    advance(); // skip 'autoindex'
    if (current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected 'on' or 'off' after 'autoindex'");
    
    std::string value = current().value;
    advance();
    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after autoindex");
    advance();

    if (value == "on")
        return true;
    if (value == "off")
        return false;
    throw std::runtime_error("Invalid value for autoindex: " + value);
}

std::vector<std::string> Parser::parseAllowedMethodsDirective() {
    advance(); // skip 'allowed_methods'
    std::vector<std::string> methods;
    while (current().type == TOKEN_IDENTIFIER) {
        methods.push_back(current().value);
        advance();
    }
    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after allowed_methods directive");
    advance();
    return methods;
}

std::map<int, std::string> Parser::parseErrorPagesDirective() {
    advance(); // skip 'error_page'
    std::map<int, std::string> pages;

    while (current().type == TOKEN_NUMBER) {
        int code = std::atoi(current().value.c_str());
        advance();

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
            throw std::runtime_error("Expected error page path after code");

        std::string path = current().value;
        advance();

        pages.insert(std::make_pair(code, path));
    }

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after error_page directive");
    advance();

    return pages;
}

size_t Parser::parseClientMaxBodySizeDirective() {
    advance(); // skip 'client_max_body_size'

    if (current().type != TOKEN_NUMBER && current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected number or size with suffix after client_max_body_size");

    std::string raw = current().value;
    advance();

    // Extraire la partie numérique et le suffixe
    std::string numberPart;
    char suffix = '\0';

    for (size_t i = 0; i < raw.length(); ++i) {
        if (std::isdigit(raw[i]))
            numberPart += raw[i];
        else {
            suffix = std::tolower(raw[i]);
            break;
        }
    }

    if (numberPart.empty())
        throw std::runtime_error("Invalid client_max_body_size value");

    long base = std::atol(numberPart.c_str());
    if (base < 0)
        throw std::runtime_error("client_max_body_size cannot be negative");

    size_t multiplier = 1;
    if (suffix == 'k')
        multiplier = 1024;
    else if (suffix == 'm')
        multiplier = 1024 * 1024;
    else if (suffix == 'g')
        multiplier = 1024 * 1024 * 1024;
    else if (suffix != '\0')
        throw std::runtime_error("Invalid suffix for client_max_body_size (use k, m, g)");

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after client_max_body_size directive");
    advance();

    return static_cast<size_t>(base) * multiplier;
}

std::string Parser::parseDefaultFileDirective()
{
    advance(); // skip 'default_file'

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected a file name after 'default_file'");

    std::string filename = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after default_file value");
    advance();

    return filename;
}


std::map<std::string, RouteConfig> Parser::parseLocationBlocks() {
    std::map<std::string, RouteConfig> routes;

    while(current().type == TOKEN_IDENTIFIER && current().value == "location") {
        advance(); // skip 'location'

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
            throw std::runtime_error("Expected path after 'location'");
        std::string path = current().value;
        advance();

        if (current().type != TOKEN_LBRACE)
            throw std::runtime_error("Expected '{' to begin location block");
        advance();

        RouteConfig route;
        while (current().type != TOKEN_RBRACE && current().type != TOKEN_EOF) {
            if (current().value == "root") {
                route.root = parseRootDirective();
            }
            else if (current().value == "autoindex") {
                route.autoindex = parseAutoindexDirective();
            }
            else {
                throw std::runtime_error("Unknown directive in location block: " + current().value);
            }
        }

        if (current().type != TOKEN_RBRACE)
            throw std::runtime_error("Expected '}' to close location block");
        advance();
        routes.insert(std::make_pair(path, route));
    }

    return routes;
}

ServerConfig Parser::parseServerBlock() {
    ServerConfig config;

    if (current().type != TOKEN_IDENTIFIER || current().value != "server")
        throw std::runtime_error("Expected 'server'");

    advance();
    if (current().type != TOKEN_LBRACE)
        throw std::runtime_error("Expected '{' after 'server'");
    advance();

    while (current().type != TOKEN_RBRACE && current().type != TOKEN_EOF) {
        if (current().type != TOKEN_IDENTIFIER)
            throw std::runtime_error("Unexpected token in server block: " + current().value);

        if (current().value == "listen") {
            config.port = parseListenDirective();
        }
        else if (current().value == "server_name") {
            config.serverNames = parseServerNameDirective();
        }
        else if (current().value == "root") {
            config.root = parseRootDirective();
        }
        else if (current().value == "autoindex") {
            config.autoindex = parseAutoindexDirective();
        }
        else if (current().value == "location") {
            std::map<std::string, RouteConfig> routes = parseLocationBlocks();
            config.routes.insert(routes.begin(), routes.end());
        }
        else {
            throw std::runtime_error("Unknown directive in server block: " + current().value);
        }
    }

    if (current().type != TOKEN_RBRACE)
        throw std::runtime_error("Expected '}' to close server block");
    advance();

    return config;
}

RouteConfig::RouteConfig()
    : path("/"),
      autoindex(false),
      uploadEnabled(false),
      returnCode(0)
{}

ServerConfig::ServerConfig()
    : port(8080),
      host("0.0.0.0"),
      autoindex(false),
      client_max_body_size(1000000)
{}