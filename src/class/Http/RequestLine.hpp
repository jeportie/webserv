/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:20:17 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/02 18:07:54 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#define REQUESTL_CONST "RequestLine constructor called"

#include <string>
#include "HttpRequest.hpp"

struct RequestLine
{
    RequestLine();

    HttpRequest::Method method;      // Il faut savoir quel verbe exécuter,
    std::string         target;      // sur quoi,
    int                 http_major;  // et selon quelle version du protocole
    // (certaines réponses ou headers se comportent différemment entre 1.0 et 1.1).
    int http_minor;
};

#endif  // ************************************************** REQUESTLINE_HPP //
