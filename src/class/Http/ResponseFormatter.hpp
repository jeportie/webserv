/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseFormatter.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:50:47 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/09 15:27:48 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEFORMATTER_HPP
#define RESPONSEFORMATTER_HPP

#include "HttpResponse.hpp"
#include <string>

class ResponseFormatter
{
public:
    ResponseFormatter(HttpResponse response);
    // std::string format();
    std::string formatHeadersOnly(const HttpResponse& resp);


private:
    HttpResponse _rep;
};
#endif
