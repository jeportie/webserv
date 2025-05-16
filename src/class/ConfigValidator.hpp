/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:19:05 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 15:22:46 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_VALIDATOR_HPP
# define CONFIG_VALIDATOR_HPP

# include "ServerConfig.hpp"
# include <set>
class ConfigValidator
{
   public:
    ConfigValidator();
    ~ConfigValidator();
    void validate(const std::vector<ServerConfig>& servers);
    void validateServer(const ServerConfig& config);
    void validateRoute(const RouteConfig& route);
    
};
#endif