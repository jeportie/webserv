/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:14 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/13 16:19:16 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "../../../include/webserv.h"

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
	LOG_ERROR(DEBUG, HTTP_REQ_ERROR, "HttpRequest constructor called.",
		"HttpRequest::HttpRequest");
}
