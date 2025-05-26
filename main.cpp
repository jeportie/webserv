/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:49:58 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/26 12:35:33 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/webserv.h"
#include "src/class/SocketManager/SocketManager.hpp"
#include "src/class/Errors/ErrorHandler.hpp"


#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

#define  Start "Server starting on port "
#define  RunT "Runtime Error: "
#define  Excp "Exception Error: "
#define  Ukwn "Unknown Error: "
#define  Crit "Critical error occurred, shutting down server"



std::string readFileToString(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    std::stringstream ss;

    if (!file.is_open())
    {
        ss << "readFileToString: cannot open file:" << filename;
        THROW_ERROR(ERROR, CONFIG_FILE_ERROR, ss.str(), __FUNCTION__);
    }

    std::string content;
    char        c;

    while (file.get(c))
    {
        content += c;
    }

    if (file.bad())
    {
        ss << "readFileToString: error while reading file: " << filename;
        THROW_ERROR(ERROR, CONFIG_FILE_ERROR, ss.str(), __FUNCTION__);
    }

    return content;
}

SSCMAP ReadandParseConfigFile(std::string& content)
{
    Lexer  lexi(content);
    Parser configData(lexi);
    SSCMAP configs;

    configs = configData.ParseConfigFile();
    return (configs);
}

int main(int argc, char **argv)

{
    std::stringstream ss;
    std::string content;
    SocketManager theSocketMaster;
    
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [config_file.conf]" << std::endl;
        return 1;
    }
    
    // Initialize error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    errorHandler.setLogLevel(DEBUG);
    errorHandler.setLogFile("webserv.log");
    
    try
    {
        
        content = readFileToString(argv[1]);
        theSocketMaster.instantiateConfig(content);
                
        // nombre de server ?
        
        std::cout << "Starting webserv on port " << PORT << std::endl;

        ss << PORT;
        LOG_ERROR(INFO, INTERNAL_ERROR, Start + ss.str(), "main");

		// Start the server
        theSocketMaster.init_connect();
    }
    catch (const std::runtime_error& e)
    {
        LOG_ERROR(CRITICAL, INTERNAL_ERROR, RunT + e.what(), "main");
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(CRITICAL, INTERNAL_ERROR, Excp + e.what(), "main");
        std::cerr << "Exception Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        LOG_ERROR(CRITICAL, INTERNAL_ERROR, Ukwn, "main");
        std::cerr << "Unknown Error" << std::endl;
    }
    
    // Check if we should shut down due to critical errors
    if (errorHandler.shouldShutdown())
    {
		errorHandler.logSystemError(CRITICAL, INTERNAL_ERROR, Crit);
        std::cerr << Crit << std::endl;
		exit(EXIT_FAILURE);
    }
    std::cout << "Server shutdown without errors" << PORT << std::endl;
    return (0);
}
