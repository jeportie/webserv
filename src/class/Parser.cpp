/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:21:43 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 16:55:03 by fsalomon         ###   ########.fr       */
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

void Parser::parseListenDirective(std::string& host, int& port) {
    advance(); // skip 'listen'

    if (current().type != TOKEN_NUMBER && current().type != TOKEN_IDENTIFIER && current().type != TOKEN_STRING)
        throw std::runtime_error("Expected host or port after 'listen'");

    std::string hostCandidate = current().value;
    advance();

    // Si le token suivant est une string avec des points, on suppose que c'est une IP continue
    if (current().type == TOKEN_STRING) {
        hostCandidate +=  current().value;
        advance();
    }

    // Si ':' alors on attend un port
    if (current().type == TOKEN_COLON) {
        host = hostCandidate;
        advance(); // skip ':'

        if (current().type != TOKEN_NUMBER)
            throw std::runtime_error("Expected port number after ':' in 'listen' directive");

        port = std::atoi(current().value.c_str());
        advance();
    } else {
        // Sinon, c’est soit juste un port (ex: `listen 8080;`)
        // soit un host (ex: `listen localhost;`)
        if (std::isdigit(hostCandidate[0])) {
            port = std::atoi(hostCandidate.c_str());
            host = "0.0.0.0"; // par défaut
        } else {
            host = hostCandidate;
            port = 80; // par défaut
        }
    }

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after listen directive");

    advance(); // skip ';'
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

std::pair<std::string, std::string> Parser::parseCgiExecutorsDirective() {
    advance(); // skip 'cgi_executor'

    if ((current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER) || current().value[0] != '.')
        throw std::runtime_error("Expected extension starting with '.' after 'cgi_executor'");
    
    std::string extension = current().value;
    advance();

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected path to CGI executable after extension");

    std::string executable = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after cgi_executor directive");
    advance();

    std::pair<std::string, std::string> result;
    result.first = extension;
    result.second = executable;
    return result;
}

std::map<int, std::string> Parser::parseReturnDirective() {
    advance(); // skip 'return'
    
    // Vérifier si le token suivant est un nombre, sinon lever une exception
    if (current().type != TOKEN_NUMBER)
    throw std::runtime_error("Expected return code (number) after 'return'");

    std::map<int, std::string> returnCodes;
    while (current().type == TOKEN_NUMBER) {
        int code = std::atoi(current().value.c_str());
        
        advance();

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
            throw std::runtime_error("Expected return URL after code");

        std::string url = current().value;
        advance();

        if (current().type != TOKEN_SEMICOLON)
            throw std::runtime_error("Expected ';' after return directive");

        returnCodes.insert(std::make_pair(code, url));
        advance();
    }

    return returnCodes;
}

bool Parser::parseUploadEnabledDirective() {
    advance(); // skip 'upload_enable'

    if (current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected 'on' or 'off' after 'upload_enable'");

    std::string value = current().value;
    advance();

    if (value != "on" && value != "off")
        throw std::runtime_error("Invalid value for 'upload_enable': " + value);

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after 'upload_enable' directive");
    advance();

    return value == "on";
}

std::string Parser::parseUploadStoreDirective() {
    advance(); // skip 'upload_store'

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        throw std::runtime_error("Expected path after 'upload_store'");

    std::string path = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
        throw std::runtime_error("Expected ';' after 'upload_store' directive");
    advance();

    return path;
}


std::map<std::string, RouteConfig> Parser::parseLocationBlocks() {
    std::map<std::string, RouteConfig> routes;

    while (current().type == TOKEN_IDENTIFIER && current().value == "location") {
        advance(); // skip 'location'

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
            throw std::runtime_error("Expected path after 'location'");

        std::string path = current().value;
        advance();

        if (current().type != TOKEN_LBRACE)
            throw std::runtime_error("Expected '{' to begin location block");
        advance();

        RouteConfig route;
        route.path = path;

        while (current().type != TOKEN_RBRACE && current().type != TOKEN_EOF) {
            if (current().type != TOKEN_IDENTIFIER)
                throw std::runtime_error("Expected directive inside location block");

            std::string directive = current().value;

            if (directive == "root") {
                route.root = parseRootDirective();
            }
            else if (directive == "autoindex") {
                route.autoindex = parseAutoindexDirective();
            }
            else if (directive == "allowed_methods") {
                route.allowedMethods = parseAllowedMethodsDirective();
            }
            else if (directive == "default_file") {
                route.defaultFile = parseDefaultFileDirective();
            }
            else if (directive == "return") {
                route.returnCodes = parseReturnDirective();
            }
            else if (directive == "cgi_executor") {
                route.cgiExecutor = parseCgiExecutorsDirective();
            }
            else if (directive == "upload_enable") {
                route.uploadEnabled = parseUploadEnabledDirective();
            }
            else if (directive == "upload_store") {
                route.uploadStore = parseUploadStoreDirective();
            }
            else {
                throw std::runtime_error("Unknown directive in location block: " + directive);
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
            parseListenDirective(config.host, config.port);
            config.listenIsSet = true;
        }
        else if (current().value == "server_name") {
            config.serverNames = parseServerNameDirective();
        }
        else if (current().value == "root") {
            config.root = parseRootDirective();
            config.rootIsSet = true;
        }
        else if (current().value == "autoindex") {
            config.autoindex = parseAutoindexDirective();
        }
       else if (current().value == "allowed_methods") {
            config.allowedMethods = parseAllowedMethodsDirective();
        }
        else if (current().value == "error_page") {
            config.error_pages = parseErrorPagesDirective();
        }
        else if (current().value == "client_max_body_size") {
            config.client_max_body_size = parseClientMaxBodySizeDirective();
        }
        else if (current().value == "default_file") {
            config.defaultFile = parseDefaultFileDirective();
        }
        else if (current().value == "cgi_executor") {
            config.cgiExecutor = parseCgiExecutorsDirective();
        }
        else if (current().value == "upload_enable") {
            config.uploadEnabled = parseUploadEnabledDirective();
        }
        else if (current().value == "upload_store") {
            config.uploadStore = parseUploadStoreDirective();
        }
        else if (current().value == "location") {
            config.routes = parseLocationBlocks();
        }
        else if (current().value == "return") {

            config.returnCodes = parseReturnDirective();
        }
        else {
            throw std::runtime_error("Unknown directive in server block: " + current().value);
        }
    }

    // Vérifie qu'on ferme bien le bloc avec '}'
    if (current().type != TOKEN_RBRACE)
        throw std::runtime_error("Expected '}' to close server block");
    advance();

    return config;
}


std::map<std::string, std::vector<ServerConfig> > Parser::parseConfigFile() {
    std::map<std::string, std::vector<ServerConfig> > serversByHostPort;

    while (current().type != TOKEN_EOF) {
        if (current().type == TOKEN_IDENTIFIER && current().value == "server") {
            ServerConfig config = parseServerBlock();

            // Valeur par défaut si aucun nom n’est spécifié
            if (config.serverNames.empty()) {
                config.serverNames.push_back("default");
            }

            std::ostringstream keyStream;
            keyStream << config.host << ":" << config.port;
            std::string key = keyStream.str();

            // Ajoute ce ServerConfig à la liste associée à ce host:port
            serversByHostPort[key].push_back(config);
        } else {
            throw std::runtime_error("Only 'server' blocks are allowed at top level");
        }
    }

    return serversByHostPort;
}


