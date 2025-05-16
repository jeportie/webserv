/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:34:09 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 10:34:32 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfig.hpp"

RouteConfig::RouteConfig()
    : path(""),
      root(""),
      allowedMethods(),
      autoindex(false),
      defaultFile(""),
      returnCodes(),
      cgiExecutors(),
      uploadEnabled(false),
      uploadStore("")
{
}
