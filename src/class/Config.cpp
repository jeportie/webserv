/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 18:09:42 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/05 20:15:28 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


Config::Config(void) { _configFilePath = "exconfig.conf"; }

Config::~Config(void)
{
    if (_configFileStream.is_open())
    {
        _configFileStream.close();
    }
}

Config::Config(const Config& src) { *this = src; }

Config& Config::operator=(const Config& rhs)
{
    if (this != &rhs)
    {
        _configFilePath = rhs._configFilePath;
        _server         = rhs._server;
    }
    return *this;
}

std::string Config::getConfigFilePath(void) const { return _configFilePath; }

std::ifstream& Config::getConfigFileStream() { return _configFileStream; }

Server Config::getServer(void) const { return _server; }

bool Config::isConfigFileOpen(void) const { return _configFileStream.is_open(); }

void Config::parseConfigFile(std::string filePath)
{
    if (_configFileStream.is_open())
        _configFileStream.close();
     _configFilePath = filePath;
    _configFileStream.open(_configFilePath.c_str());
    if (!_configFileStream.is_open())
    {
        throw std::runtime_error("Failed to open config file: " + _configFilePath);
    }
    std::string line;
    while (std::getline(_configFileStream, line))
    {
        parseLine(line);
    }
}

void Config::parseLine(std::string line)
{
    if (line.find("server") != std::string::npos)
    {
        std::string nextLine;
        while (line.find("}") == std::string::npos && std::getline(_configFileStream, nextLine))
        {
            line += nextLine;
        }
        parseServerBlock(line);
    }
    else
    {
        throw std::runtime_error("Unknown directive: " + line);
    }
}

void Config::parseServerBlock(std::string block)
{
    Server server;

    std::istringstream iss(block);
    std::string        directive;
    while (iss >> directive)
    {
        if (directive == "listen")
        {
            int port;
            iss >> port;
            server.setPort(port);
        }
        else if (directive == "server_name")
        {
            std::string serverName;
            iss >> serverName;
            server.setServerName(serverName);
        }
        else if (directive == "root")
        {
            std::string root;
            iss >> root;
            server.setRoot(root);
        }
        else if (directive == "index")
        {
            std::string index;
            iss >> index;
            server.setIndex(index);
        }
        else if (directive == "error_page")
        {
            std::string errorPage;
            iss >> errorPage;
            server.setErrorPage(errorPage);
        }
        else
        {
            throw std::runtime_error("Unknown directive in server block: " + directive);
        }
    }

    std::cout << server << std::endl;
}
