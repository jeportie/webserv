/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:34:45 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 17:58:54 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
: port(80)
, host("0.0.0.0")
, serverNames()
, root("")
, allowedMethods()
, error_pages()
, client_max_body_size(1048576)
, routes()
, indexFiles()
, indexIsSet(false)
, listenIsSet(false)
, rootIsSet(false)
{
}
