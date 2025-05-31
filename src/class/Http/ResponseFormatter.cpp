/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseFormatter.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 14:36:00 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/31 14:44:22 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseFormatter.hpp"
#include <map>
#include <string>
#include "HttpResponse.hpp"

ResponseFormatter::ResponseFormatter(HttpResponse response)
    : _rep(response) {}

std::string ResponseFormatter::format()
{
    std::ostringstream oss;

    // 1. Ligne de statut
    oss << "HTTP/1.1 " << _rep.getStatusCode() << " "
        << _rep.getStatusMessage() << "\r\n";

    // 2. Headers
    std::map<std::string, std::string> headers = _rep.getAllHeaders();
    std::map<std::string, std::string>::const_iterator it = headers.begin();
    for (; it != headers.end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";

    // 3. Séparateur
    oss << "\r\n";

    // 4. Corps
    oss << _rep.getBody();

    return oss.str();
}
