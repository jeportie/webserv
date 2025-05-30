/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:17 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/30 19:57:57 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#define HTTPREQ_CONST "HttpRequest constructor called"

#include "../../../include/webserv.h"

struct HttpRequest
{
    enum Method
    {
        METHOD_GET,
        METHOD_POST,
        METHOD_PUT,
        METHOD_DELETE,
        METHOD_INVALID
    };

    HttpRequest(void);

    Method      method;
    std::string path;
    std::string raw_query;
    int         http_major;
    int         http_minor;
    SVSMAP		headers;
    std::string body;
    SSMAP       query_params;
    SSMAP       form_data;
};

#endif  // ************************************************** HTTPREQUEST_HPP //
