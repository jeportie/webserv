/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:27:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 10:34:29 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef ROUTECONFIG_HPP
# define ROUTECONFIG_HPP

# include "Parser.hpp"

struct RouteConfig {
    std::string path; // le chemin de la route, ex: /images
    std::string root;
    std::vector<std::string> allowedMethods;
    bool autoindex;
    std::string defaultFile;
    std::map<int, std::string> returnCodes;
    std::map<std::string, std::string> cgiExecutors; // ex: ".php" => "/usr/bin/php-cgi"
    bool uploadEnabled;
    std::string uploadStore;
    RouteConfig();
};


#endif 