/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:31:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 11:38:35 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* config validator doit verifier :


-verifier que les chemins dans upload store root et cgi executor sont valide
-les blocs locations dans le meme server ne doivent pas avoir exactement le meme chemin (collision)
*/

#include "ConfigValidator.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.h"

#include <sstream>
#include <set>

ConfigValidator::ConfigValidator() {};

ConfigValidator::~ConfigValidator() {};

bool isValidIPv4(const std::string& ip)
{
    std::istringstream	iss(ip);
    std::string			token;
    int					parts;
	int					value;

    parts = 0;
    while (std::getline(iss, token, '.'))
    {
        if (parts >= 4)
            return false;

        // Vérifie que chaque token est composé uniquement de chiffres
        if (token.empty() || token.find_first_not_of("0123456789") != std::string::npos)
            return false;

        // Convertit le token en entier
        value = std::atoi(token.c_str());
        if (value < 0 || value > 255)
            return false;

        // Empêche les zéros en tête (ex: "01" est invalide sauf pour "0")
        if (token.length() > 1 && token[0] == '0')
            return false;

        ++parts;
    }
    return parts == 4;
}


void ConfigValidator::validate(const std::vector<ServerConfig>& servers)
{
    std::string error_msg;
    std::set<std::string> listenSet;
	std::vector<ServerConfig>::const_iterator it;
    std::ostringstream oss;
	std::string key;

    if (servers.empty())
		THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, "No server block defined", __FUNCTION__);
	

    for (size_t i = 0; i < servers.size(); ++i)
    {
        validateServer(servers[i]);
    }

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        oss << it->host << ":" << it->port;
        key = oss.str();

        if (!listenSet.insert(key).second)
        {
			error_msg = "Duplicate host:port detected: " + key;
			THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
    }
}

void ConfigValidator::validateServer(const ServerConfig& config)
{
    std::string error_msg;
    std::set<std::string> validMethods;

    if (!config.listenIsSet)
    {
        error_msg = "Missing required directive: listen";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }
    if (!config.rootIsSet)
    {
        error_msg = "Missing required directive: root";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (config.port <= 0 || config.port > 65535)
    {
        error_msg = "Invalid port number in server config";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (config.host.empty())
    {
        error_msg = "Host must be defined in server config";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (!isValidIPv4(config.host))
    {
        error_msg = "Host must be a valid IPv4";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (config.root.empty())
    {
        error_msg = "Root directory must be defined in server config or at least in every route";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (config.client_max_body_size <= 0)
    {
        error_msg = "client_max_body_size must be greater than 0";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    for (std::map<int, std::string>::const_iterator it = config.error_pages.begin();
         it != config.error_pages.end();
         ++it)
    {
        if (it->first < 400 || it->first > 599)
        {
            error_msg = "Invalid HTTP status code in error_page";
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }    
        if (it->second.empty())
        {
            error_msg = "error_page URL must not be empty";
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }    
    }

    validMethods.insert("GET");
    validMethods.insert("POST");
    validMethods.insert("DELETE");

    for (size_t i = 0; i < config.allowedMethods.size(); ++i)
    {
        if (validMethods.find(config.allowedMethods[i]) == validMethods.end())
        { 
            error_msg = "Invalid allowed method: " + config.allowedMethods[i];
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }
    }

    for (std::map<std::string, RouteConfig>::const_iterator it = config.routes.begin();
         it != config.routes.end();
         ++it)
    {
        validateRoute(it->second);
    }
}


void ConfigValidator::validateRoute(const RouteConfig& route)
{
    std::string error_msg;
    
    if (route.path.empty())
    {
        error_msg = "Route path is required";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (route.root.empty() && route.returnCodes.empty())
    {
        error_msg = "Each route must have either a root or a return directive";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (!route.root.empty() && !route.returnCodes.empty())
    {
        error_msg = "A route cannot have both root and return directives at the same time";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (!route.defaultFile.empty() && route.defaultFile.find('/') == 0)
    {
        error_msg = "default_file must be a relative filename (not starting with /)";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (route.defaultFile.find('/') != std::string::npos)
    {
        error_msg = "default_file must not contain '/'";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    for (std::map<int, std::string>::const_iterator it = route.returnCodes.begin();
         it != route.returnCodes.end();
         ++it)
    {
        if (it->first < 100 || it->first > 599)
        {
        error_msg = "Invalid HTTP status code";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }    
        if (it->second.empty())
        {
            error_msg = "return_url must not be empty for return_code";
            THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
        }    
    }
    if (!route.cgiExecutor.first.empty() && route.cgiExecutor.second.find('/') != 0)
    {
        error_msg = "cgi_executor must be an absolute path";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
    if (route.uploadEnabled && route.uploadStore.empty())
    {
        error_msg = "upload_store must be set if upload is enabled";
        THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, error_msg, __FUNCTION__);
    }    
}
