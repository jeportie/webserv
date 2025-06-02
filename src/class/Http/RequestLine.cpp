/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:20:46 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/02 18:07:50 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestLine.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"

RequestLine::RequestLine()
: method(HttpRequest::METHOD_INVALID)
, target()
, http_major(0)
, http_minor(0)
{
    LOG_ERROR(DEBUG, HTTP_REQ_ERROR, REQUESTL_CONST, __FUNCTION__);
}
