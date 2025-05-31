/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseFormatter.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:50:47 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/31 14:44:07 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEFORMATTER_HPP
#define RESPONSEFORMATTER_HPP

#include "HttpResponse.hpp"
#include <string>
#include <sstream>

class ResponseFormatter
{
public:
    ResponseFormatter(HttpResponse response);
    std::string format();

private:
    HttpResponse _rep;
};
#endif
