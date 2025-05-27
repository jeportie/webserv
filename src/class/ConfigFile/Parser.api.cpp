/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.api.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 08:50:28 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 09:25:01 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

void Parser::parseListenDirective(std::string& host, int& port)
{
    std::string hostCandidate;
    std::string error_msg;
    
    advance();  // skip 'listen'

    if (current().type != TOKEN_NUMBER && current().type != TOKEN_IDENTIFIER
        && current().type != TOKEN_STRING)
    {
        error_msg = "Expected host or port after 'listen'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    hostCandidate = current().value;
    advance();
    // Si le token suivant est une string avec des points, on suppose que c'est une IP continue
    if (current().type == TOKEN_STRING)
    {
        hostCandidate += current().value;
        advance();
    }
    // Si ':' alors on attend un port
    if (current().type == TOKEN_COLON)
    {
        host = hostCandidate;
        advance();  // skip ':'
        if (current().type != TOKEN_NUMBER)
        {
            error_msg = "Expected port number after ':' in 'listen' directive";
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
        port = std::atoi(current().value.c_str());
        advance();
    }
    else
    {
        // Sinon, c’est soit juste un port (ex: `listen 8080;`)
        // soit un host (ex: `listen localhost;`)
        if (std::isdigit(hostCandidate[0]))
        {
            port = std::atoi(hostCandidate.c_str());
            host = "0.0.0.0";  // par défaut
        }
        else
        {
            host = hostCandidate;
            port = 80;  // par défaut
        }
    }
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after listen directive";
	    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();  // skip ';'
}


std::vector<std::string> Parser::parseServerNameDirective()
{
    std::vector<std::string> names;
    std::string error_msg;
    
    advance();  // skip 'server_name'

    while (current().type == TOKEN_IDENTIFIER || current().type == TOKEN_STRING)
    {
        names.push_back(current().value);
        advance();
    }

    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after server_name values";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    return names;
}


std::string Parser::parseRootDirective()
{
    std::string error_msg;
    std::string root;
    
    advance();  // skip 'root'
    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected path after 'root'";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    root = current().value;

    advance();
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after root";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    
    return root;
}


std::vector<std::string> Parser::parseAllowedMethodsDirective()
{
    std::vector<std::string> methods;
    std::string error_msg;
    
    advance();  // skip 'allowed_methods'
    while (current().type == TOKEN_IDENTIFIER)
    {
        methods.push_back(current().value);
        advance();
    }
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after allowed_methods directive";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    
    return methods;
}

std::map<int, std::string> Parser::parseErrorPagesDirective()
{
    std::map<int, std::string> pages;
    std::string error_msg;
    std::String path;
    
    advance();  // skip 'error_page'

    while (current().type == TOKEN_NUMBER)
    {
        int code = std::atoi(current().value.c_str());
        advance();

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        {
            error_msg = "Expected error page path after code";
		    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }

        path = current().value;
        advance();

        pages.insert(std::make_pair(code, path));
    }

    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after error_page directive";
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();
    
    return pages;
}

size_t Parser::parseClientMaxBodySizeDirective()
{
    std::string raw;
    std::string numberPart;
    std::string error_msg;
    long base;
    size_t multiplier;
    char suffix;

    
    advance();  // skip 'client_max_body_size'
    if (current().type != TOKEN_NUMBER && current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected number or size with suffix after client_max_body_size";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }        
    raw = current().value;
    advance();
    // Extraire la partie numérique et le suffixe
    suffix = '\0';
    for (size_t i = 0; i < raw.length(); ++i)
    {
        if (std::isdigit(raw[i]))
            numberPart += raw[i];
        else
        {
            suffix = std::tolower(raw[i]);
            break;
        }
    }
    if (numberPart.empty())
    {
        error_msg = "Invalid client_max_body_size value";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }        
    base = std::atol(numberPart.c_str());
    if (base < 0)
    {
        error_msg = "client_max_body_size cannot be negative";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }        
    multiplier = 1;
    if (suffix == 'k')
        multiplier = 1024;
    else if (suffix == 'm')
        multiplier = 1024 * 1024;
    else if (suffix == 'g')
        multiplier = 1024 * 1024 * 1024;
    else if (suffix != '\0')
    {
        error_msg = "Invalid suffix for client_max_body_size (use k, m, g)";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }        
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after client_max_body_size directive";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    return static_cast<size_t>(base) * multiplier;
}


// LOCATION BLOC DIRECTIVE
// ******************************************************************************************


bool Parser::parseAutoindexDirective()
{
    std::string error_msg;
    std::string value;

    advance();  // skip 'autoindex'
    if (current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected 'on' or 'off' after 'autoindex'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }

    value = current().value;
    advance();
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after autoindex";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    if (value == "on")
        return true;
    if (value == "off")
        return false;
    error_msg = "Invalid value for autoindex: " + value;
    THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
}


std::string Parser::parseDefaultFileDirective()
{
    std::string error_msg;
    std::string filename;
    
    advance();  // skip 'default_file'

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected a file name after 'default_file'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }

    filename = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after default_file value";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    return filename;
}

std::map<int, std::string> Parser::parseReturnDirective()
{
    std::string error_msg;
    std::map<int, std::string> returnCodes;
    int code;
    std::string url;
    
    advance();  // skip 'return'
    // Vérifier si le token suivant est un nombre, sinon lever une exception
    if (current().type != TOKEN_NUMBER)
    {
        error_msg = "Expected return code (number) after 'return'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    while (current().type == TOKEN_NUMBER)
    {
        code = std::atoi(current().value.c_str());

        advance();

        if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        {
            error_msg = "Expected return URL after code";
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
        url = current().value;
        advance();

        if (current().type != TOKEN_SEMICOLON)
        {
            error_msg = "Expected ';' after return directive";
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }

        returnCodes.insert(std::make_pair(code, url));
        advance();
    }

    return returnCodes;
}

std::pair<std::string, std::string> Parser::parseCgiExecutorsDirective()
{
    std::string error_msg;
    std::string extension;
    std::string executable;
    std::pair<std::string, std::string> result;
    
    advance();  // skip 'cgi_executor'

    if ((current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
        || current().value[0] != '.')
    {
        error_msg = "Expected extension starting with '.' after 'cgi_executor'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }

    extension = current().value;
    advance();

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected path to CGI executable after extension";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    executable = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after cgi_executor directive";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    result.first  = extension;
    result.second = executable;
    return result;
}


bool Parser::parseUploadEnabledDirective()
{
    std::string error_msg;
    std::string value;
    
    advance();  // skip 'upload_enable'

    if (current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected 'on' or 'off' after 'upload_enable'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }

    value = current().value;
    advance();

    if (value != "on" && value != "off")
    {
        error_msg = "Invalid value for 'upload_enable': " + value;
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after 'upload_enable' directive";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    return value == "on";
}

std::string Parser::parseUploadStoreDirective()
{
    std::string error_msg;
    std::string path;
    advance();  // skip 'upload_store'

    if (current().type != TOKEN_STRING && current().type != TOKEN_IDENTIFIER)
    {
        error_msg = "Expected path after 'upload_store'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }

    path = current().value;
    advance();

    if (current().type != TOKEN_SEMICOLON)
    {
        error_msg = "Expected ';' after 'upload_store' directive";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    advance();

    return path;
}
