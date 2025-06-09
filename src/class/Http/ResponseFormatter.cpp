/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseFormatter.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 14:36:00 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/09 15:27:42 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseFormatter.hpp"
#include <map>
#include <string>
#include <sstream>

#include "HttpResponse.hpp"

ResponseFormatter::ResponseFormatter(HttpResponse response)
: _rep(response)
{
}

std::string ResponseFormatter::formatHeadersOnly(const HttpResponse& resp)
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << resp.getStatusCode() << " " << resp.getStatusMessage() << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = resp.getAllHeaders().begin(); it != resp.getAllHeaders().end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";
    oss << "\r\n";
    return oss.str();
}


// std::string ResponseFormatter::format()
// {
//     std::ostringstream oss;

//     // 1. Ligne de statut
//     oss << "HTTP/1.1 " << _rep.getStatusCode() << " " << _rep.getStatusMessage() << "\r\n";

//     // 2. Headers
//     std::map<std::string, std::string>                 headers = _rep.getAllHeaders();
//     std::map<std::string, std::string>::const_iterator it      = headers.begin();
//     for (; it != headers.end(); ++it)
//         oss << it->first << ": " << it->second << "\r\n";

//     if (headers.find("Content-Length") == headers.end())
//         oss << "Content-Length: " << _rep.getBody().size() << "\r\n";

//     // 3. Séparateur
//     oss << "\r\n";

//     // 4. Corps
//     oss << _rep.getBody();

//     return oss.str();
// }
