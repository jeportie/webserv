/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:19:05 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 17:59:33 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "ServerConfig.hpp"

class ConfigValidator
{
public:
    ConfigValidator();
    ~ConfigValidator();

    void validate(const std::vector<ServerConfig>& servers);
    void validateServer(const ServerConfig& config);
    void validateRoute(const RouteConfig& route);
};

#endif  // ********************************************** CONFIGVALIDATOR_HPP //
