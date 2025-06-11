/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidator.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 15:11:43 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/10 17:26:58 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTVALIDATOR_HPP
#define REQUESTVALIDATOR_HPP

#include <cctype>  // isspace, isdigit
#include <cstring> // strerror
#include <cstdlib> // pour strtol

#include "../ConfigFile/ServerConfig.hpp"
#include "HttpRequest.hpp"

class RequestValidator
{
public:
    RequestValidator(const HttpRequest &req, const ServerConfig &serverConfig);
    ~RequestValidator();
    

    void validate(); // throw HttpException si erreur

    // Renvoie le chemin de la page d’erreur custom (ou vide si aucune)
    std::string resolvePath(std::string relativePath, std::string root);
    std::string getErrorPage(int statusCode);
    RouteConfig getMatchedRoute() const;
    bool hasMatchedRoute() const;
    ServerConfig getServerConfig() const;

    // Retourne la RouteConfig matchée ou NULL
    const RouteConfig *matchedRoute() const;
    std::string matchedPrefix() const;

private:
    const HttpRequest &_req;
    const ServerConfig &_serverConfig;
    const RouteConfig *_matchedRoute; // null si fallback server
    std::string _matchedPrefix;

    void matchRoute(); // détermine la route (modifie _matchedRoute)
    void validateMethod();
    void validateHeaders();
    void validateBody();
};

#endif
