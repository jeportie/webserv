/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readFileToString.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:37:03 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/10 15:57:00 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>

#include "../include/webserv.hpp"
#include "class/Errors/ErrorHandler.hpp"

std::string readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    std::stringstream ss;


    if (!file.is_open())
    {
        ss << "readFileToString: cannot open file:" << filename;
        THROW_ERROR(ERROR, CONFIG_FILE_ERROR, ss.str(), __FUNCTION__);
    }

    std::string content;
    content = "";
    char        c;

    while (file.get(c))
    {
        content += c;
    }

    if (file.bad())
    {
        ss << "readFileToString: error while reading file: " << filename;
        THROW_ERROR(ERROR, CONFIG_FILE_ERROR, ss.str(), __FUNCTION__);
    }

    return content;
}

