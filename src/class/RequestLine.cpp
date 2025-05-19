/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:20:46 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/13 16:20:52 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestLine.hpp"

RequestLine::RequestLine()
  : method(HttpRequest::METHOD_INVALID)
  , target()
  , http_major(0)
  , http_minor(0)
{}
