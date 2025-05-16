/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:27:50 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 11:18:47 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "Parser.hpp"
# include "RouteConfig.hpp" 

struct RouteConfig;


struct ServerConfig {
    int port;
    std::string host;
    std::vector<std::string> serverNames;
    std::string root;
    std::vector<std::string> allowedMethods;
    bool autoindex;
    std::string defaultFile;
    std::map<int, std::string> returnCodes;
    std::map<int, std::string>      error_pages;             // ex: {404: "/errors/404.html"}
    size_t                          client_max_body_size;    // en octets
    std::map<std::string, std::string> cgiExecutors;
    bool uploadEnabled;
    std::string uploadStore;
    std::map<std::string, RouteConfig> routes;
    // Autres directives comme error_pages, cgi, etc.

    ServerConfig();
};

 
#endif 