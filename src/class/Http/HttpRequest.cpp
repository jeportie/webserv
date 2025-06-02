/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:03:14 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/02 18:02:28 by jeportie         ###   ########.fr       */
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
