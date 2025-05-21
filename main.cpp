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
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

int main()
{
    // Initialize error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    // Set min log level (DEBUG TO SHOW DEBUG LOGS)
    errorHandler.setLogLevel(DEBUG);
	// Create the log file
    errorHandler.setLogFile("webserv.log");
    
    try
    {
        std::cout << "Starting webserv on port " << PORT << std::endl;
        SocketManager theSocketMaster;

        // Log server start
        std::stringstream ss;
        ss << PORT;
        errorHandler.logError(INFO, INTERNAL_ERROR, "Server starting on port " + ss.str(), "main");

		// Start the server
        theSocketMaster.init_connect();
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
		errorHandler.logSystemError(CRITICAL, INTERNAL_ERROR, "Critical error occurred, shutting down server");
        std::cerr << "Critical error occurred, shutting down server" << std::endl;
		exit(EXIT_FAILURE);
    }
    return (0);
}
