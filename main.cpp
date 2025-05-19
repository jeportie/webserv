/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:49:58 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/08 01:04:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/webserv.h"
#include "src/class/SocketManager.hpp"
#include "src/class/ErrorHandler.hpp"
#include "src/class/ConfigValidator.hpp"
#include "src/class/Parser.hpp"
#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
    // Initialize error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    // Set log level and log file
    errorHandler.setLogLevel(INFO);
    errorHandler.setLogFile("webserv.log");
    
 try
    {
        if (argc < 2)
			throw std::runtime_error("Format: ./webserv <config_file.conf>");

		std::string configPath = argv[1];
        // Read configuration file
        std::ifstream configFile(configPath.c_str());
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open configuration file: " + configPath);
        }
        
        // Read file content into string
        std::stringstream buffer;
        buffer << configFile.rdbuf();
        std::string content = buffer.str();
        
        // Parse configuration
        Lexer lexer(content);
        Parser parser(lexer);
        std::map<std::string, std::vector<ServerConfig>> serversByHostPort = parser.parseConfigFile();
        
        // Extract all server configs into a flat vector
        std::vector<ServerConfig> serverConfigs;
        for (std::map<std::string, std::vector<ServerConfig>>::iterator it = serversByHostPort.begin();
             it != serversByHostPort.end(); ++it) {
            for (size_t i = 0; i < it->second.size(); ++i) {
                serverConfigs.push_back(it->second[i]);
            }
        }
        
        // Validate configuration
        ConfigValidator validator;
        validator.validate(serverConfigs);
        
        // Initialize socket manager with configuration
        SocketManager socketManager;
        
        // Log server start
        errorHandler.logError(INFO, INTERNAL_ERROR, 
                             "Server starting with configuration from " + configPath, "main");
        
        // Initialize server with configuration
        socketManager.init_connect_with_config(serverConfigs);
    }
    catch (const std::runtime_error& e)
    {
        errorHandler.logError(CRITICAL, INTERNAL_ERROR, std::string("Runtime Error: ") + e.what(), "main");
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        errorHandler.logError(CRITICAL, INTERNAL_ERROR, std::string("Exception Error: ") + e.what(), "main");
        std::cerr << "Exception Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        errorHandler.logError(CRITICAL, INTERNAL_ERROR, "Unknown Error", "main");
        std::cerr << "Unknown Error" << std::endl;
    }
    
    // Check if we should shut down due to critical errors
    if (errorHandler.shouldShutdown())
    {
        std::cerr << "Critical error occurred, shutting down server" << std::endl;
    }
    
    return (0);
}
