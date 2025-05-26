/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:31:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/19 13:15:24 by fsalomon         ###   ########.fr       */
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
    std::set<std::string> listenSet;
	std::vector<ServerConfig>::const_iterator it;
    std::ostringstream oss;
    std::ostringstream msg;
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
			msg << "Duplicate host:port detected: " << key;
			THROW_SYSTEM_ERROR(CRITICAL, CONFIG_FILE_ERROR, msg.str(), __FUNCTION__);
        }
    }
}

void ConfigValidator::validateServer(const ServerConfig& config)
{
    if (!config.listenIsSet)
        throw std::runtime_error("Missing required directive: listen");

    if (!config.rootIsSet)
        throw std::runtime_error("Missing required directive: root");
    if (config.port <= 0 || config.port > 65535)
        throw std::runtime_error("Invalid port number in server config");
    if (config.host.empty())
        throw std::runtime_error("Host must be defined in server config");

    if (!isValidIPv4(config.host))
        throw std::runtime_error("Host must be a valid IPv4");

    if (config.root.empty())
        throw std::runtime_error(
            "Root directory must be defined in server config or at least in every route");

    if (config.client_max_body_size <= 0)
        throw std::runtime_error("client_max_body_size must be greater than 0");

    for (std::map<int, std::string>::const_iterator it = config.error_pages.begin();
         it != config.error_pages.end();
         ++it)
    {
        if (it->first < 400 || it->first > 599)
            throw std::runtime_error("Invalid HTTP status code in error_page");
        if (it->second.empty())
            throw std::runtime_error("error_page URL must not be empty");
    }

    std::set<std::string> validMethods;
    validMethods.insert("GET");
    validMethods.insert("POST");
    validMethods.insert("DELETE");

    for (size_t i = 0; i < config.allowedMethods.size(); ++i)
    {
        if (validMethods.find(config.allowedMethods[i]) == validMethods.end())
        {
            throw std::runtime_error("Invalid allowed method: " + config.allowedMethods[i]);
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
    if (route.path.empty())
        throw std::runtime_error("Route path is required");

    if (route.root.empty() && route.returnCodes.empty())
        throw std::runtime_error("Each route must have either a root or a return directive");

    if (!route.root.empty() && !route.returnCodes.empty())
        throw std::runtime_error(
            "A route cannot have both root and return directives at the same time");

    if (!route.defaultFile.empty() && route.defaultFile.find('/') == 0)
        throw std::runtime_error("default_file must be a relative filename (not starting with /)");


    if (route.defaultFile.find('/') != std::string::npos)
        throw std::runtime_error("default_file must not contain '/'");

    for (std::map<int, std::string>::const_iterator it = route.returnCodes.begin();
         it != route.returnCodes.end();
         ++it)
    {
        if (it->first < 100 || it->first > 599)
            throw std::runtime_error("Invalid HTTP status code");
        if (it->second.empty())
            throw std::runtime_error("return_url must not be empty for return_code");
    }
    if (!route.cgiExecutor.first.empty() && route.cgiExecutor.second.find('/') != 0)
        throw std::runtime_error("cgi_executor must be an absolute path");

    if (route.uploadEnabled && route.uploadStore.empty())
        throw std::runtime_error("upload_store must be set if upload is enabled");
}
