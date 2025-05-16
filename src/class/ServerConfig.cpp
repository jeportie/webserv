/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:34:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 12:10:51 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
    : port(80),
      host("0.0.0.0"),
      serverNames(),
      root(""),
      allowedMethods(),
      autoindex(false),
      defaultFile(""),
      returnCodes(),
      error_pages(),
      client_max_body_size(1048576),
      cgiExecutors(),
      uploadEnabled(false),
      uploadStore(""),
      routes()
      listenIsSet(false);
      rootIsSet(false);
{
}
