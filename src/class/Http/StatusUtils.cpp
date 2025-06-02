/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:03:38 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:09:19 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StatusUtils.hpp"
#include <map>
#include <string>

std::string getStatusMessage(int code)
{
    std::map<int, std::string> statusMessages;

    statusMessages[200] = "OK";
    statusMessages[201] = "Created";
    statusMessages[204] = "No Content";
    statusMessages[301] = "Moved Permanently";
    statusMessages[302] = "Found";
    statusMessages[400] = "Bad Request";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
    statusMessages[405] = "Method Not Allowed";
    statusMessages[413] = "Payload Too Large";
    statusMessages[500] = "Internal Server Error";
    statusMessages[501] = "Not Implemented";
    statusMessages[502] = "Bad Gateway";
    statusMessages[503] = "Service Unavailable";

    std::map<int, std::string>::const_iterator it = statusMessages.find(code);
    if (it != statusMessages.end())
        return it->second;

    return "Unknown Status";
}
