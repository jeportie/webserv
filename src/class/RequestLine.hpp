/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:20:17 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/13 16:20:19 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include <string>
#include "HttpRequest.hpp"  // pour récupérer HttpRequest::Method

struct RequestLine {
    // Constructeur par défaut déclaré
    RequestLine();

    // On réutilise **HttpRequest::Method**
    HttpRequest::Method method;
    std::string         target;
    int                 http_major;
    int                 http_minor;
};

#endif // REQUESTLINE_HPP
