/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:17 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/13 11:59:33 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <unordered_map>

struct HttpRequest
{
    enum Method { GET, POST, PUT, DELETE, INVALID};
    Method method;
    std::string path;
    std::string raw_query;
    int http_major, http_minor;
    std::unordered_map<std::string, std::string> headers; 
    std::string body;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> form_data;
};
