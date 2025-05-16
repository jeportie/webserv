/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:19:05 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 10:40:41 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_VALIDATOR_HPP
# define CONFIG_VALIDATOR_HPP

# include "ServerConfig.hpp"

class ConfigValidator
{
   public:
    ConfigValidator();
    ~ConfigValidator();
    void validate(const std::vector<ServerConfig>& servers);

   private :
   void validateServer(const ServerConfig& config);
   void validateRoute(const RouteConfig& route);
    
};

#endif