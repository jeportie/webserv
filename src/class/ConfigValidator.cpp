/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:31:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 10:50:23 by fsalomon         ###   ########.fr       */
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
    if (config.port <= 0)
        throw std::runtime_error("Invalid or missing 'listen' directive");
}