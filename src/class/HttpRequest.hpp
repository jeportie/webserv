/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:17 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/15 17:37:14 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <map>
#include <vector>

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP


#include <string>
#include <map>

struct HttpRequest {
    // 1) Déclaration de l’énum **une seule fois**
    enum Method {
        METHOD_GET,
        METHOD_POST,
        METHOD_PUT,
        METHOD_DELETE,
        METHOD_INVALID
    };

    // 2) Constructeur par défaut déclaré
    HttpRequest();

    // 3) Champs de la requête complète
    Method                                method;
    std::string                           path;
    std::string                           raw_query;
    int                                   http_major;
    int                                   http_minor;
    std::map<std::string,std::vector<std::string> >    headers;
    std::string                           body;
    std::map<std::string, std::string>    query_params;
    std::map<std::string, std::string>    form_data;
};

#endif // HTTPREQUEST_HPP
