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
#include "src/class/SocketManager/SocketManager.hpp"
#include "src/class/Errors/ErrorHandler.hpp"

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

int main()
{
	std::string Start("Server starting on port ");
	std::string RunT("Runtime Error: ");
	std::string Excp("Exception Error: ");
	std::string Ukwn("Unknown Error: ");

    // Initialize error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    errorHandler.setLogLevel(DEBUG);
    errorHandler.setLogFile("webserv.log");
    
    try
    {
        std::cout << "Starting webserv on port " << PORT << std::endl;
        SocketManager theSocketMaster;

        std::stringstream ss;
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
		errorHandler.logSystemError(CRITICAL, INTERNAL_ERROR, "Critical error occurred, shutting down server");
        std::cerr << "Critical error occurred, shutting down server" << std::endl;
		exit(EXIT_FAILURE);
    }
    return (0);
}
