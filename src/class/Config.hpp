/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 18:05:01 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/05 20:13:10 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <fstream>
#include <sstream>


class Config
{
public:
    Config(void);
    ~Config(void);

    Config(const Config& src);
    Config& operator=(const Config& rhs);

    std::string    getConfigFilePath(void) const;
    std::ifstream& getConfigFileStream();
    bool           isConfigFileOpen(void) const;
    Server         getServer(void) const;

    void parseConfigFile(std::string filePath);
    void parseLine(std::string line);
    void parseServerBlock(std::string block);


private:
    std::string   _configFilePath;
    std::ifstream _configFileStream;
    Server        _server;
};

#endif
