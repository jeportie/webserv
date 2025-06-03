/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:14 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/03 12:53:22 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

HttpRequest::HttpRequest()
: method(METHOD_INVALID)
, path()
, raw_query()
, http_major(0)
, http_minor(0)
, headers()
, body()
, query_params()
, form_data()
{
    LOG_ERROR(DEBUG, HTTP_REQ_ERROR, HTTPREQ_CONST, __FUNCTION__);
}

std::string HttpRequest::methodString() const
{	
	    switch (method)
    {
        case METHOD_GET : 
            return ("GET");
        case METHOD_DELETE : 
            return ("DELETE");
        case METHOD_POST :
			return ("POST");
		default:
            return "INVALID";
    }
}
