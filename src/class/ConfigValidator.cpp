/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:31:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 12:13:42 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



/* config validator doit verifier :

- listen pour chaque bloc server
-root doit etre defini soit dans le serveur soit dans chaque location
-si uploadenable est on upload store doit etre defini
-si return code et difini return url aussi (deja implementer dans le parser ?)
-verifier que l'IP est valide (inet_addr) ? 
-client max body size est un entier positif 
-allowed methode contient seulement POST et GET
-verifier que les chemins dans upload store root et cgi executor sont valide 
-bloc location obligatoirement dans bloc server 
-chemin d'un location doit commencer par '/'
-les servers ne doivent pas ecouter le meme port (virtualhosting)
-les blocs locations dans le meme server ne doivent pas avoir exactement le meme chemin (collision)
-un cgi executor est defini mais aucun fichier desservi par cette route ou aucune regle de declenchement de CGI par extension nest en place 
*/


#include "ConfigValidator.hpp"


ConfigValidator::ConfigValidator(){};

ConfigValidator::~ConfigValidator(){};


void ConfigValidator::validate(const std::vector<ServerConfig>& servers)
{
     if (servers.empty())
        throw std::runtime_error("No server block defined");

    for (size_t i = 0; i < servers.size(); ++i) {
        validateServer(servers[i]);
    }
}

void ConfigValidator::validateServer(const ServerConfig& config)
{
    if (listenIsSet == false || rootIsSet == false)
        throw std::runtime_error("listen and root directive are mandatory in server config")
    if (config.port <= 0 || config.port > 65535)
        throw std::runtime_error("Invalid port number in server config");
        if (config.host.empty())
        throw std::runtime_error("Host must be defined in server config");

    if (config.root.empty())
        throw std::runtime_error("Root directory must be defined in server config");

    if (config.client_max_body_size == 0)
        throw std::runtime_error("client_max_body_size must be greater than 0");
}


void ConfigValidator::validateRouteConfig(const RouteConfig& route) {
    if (route.path.empty())
        throw std::runtime_error("Route path is required");

    if (route.root.empty())
        throw std::runtime_error("Route root is required");

    // Exemples supplémentaires :
    if (route.uploadEnabled && route.uploadStore.empty())
        throw std::runtime_error("upload_store must be set if upload is enabled");

    for (std::map<int, std::string>::const_iterator it = route.returnCodeDirective.begin(); it != route.returnCodeDirective.end(); ++it) {
        if (it->second.empty())
            throw std::runtime_error("return_url must not be empty for return_code " + std::to_string(it->first));
    }
}
