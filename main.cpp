/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:49:58 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:12:56 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/webserv.hpp"
#include "src/class/SocketManager/SocketManager.hpp"
#include "src/class/Errors/ErrorHandler.hpp"

#include <cstdlib>
#include <stdexcept>
#include <iostream>

std::string Start("Server starting...");
std::string RunT("Runtime Error: ");
std::string Excp("Exception Error: ");
std::string Ukwn("Unknown Error: ");
std::string Crit("Critical error occurred, shutting down server");

int main(int argc, char** argv)

{
    std::string   content;
    SocketManager theSocketMaster;

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [config_file.conf]" << std::endl;
        return (1);
    }
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    errorHandler.setLogLevel(DEBUG);
    errorHandler.setLogFile("webserv.log");

    try
    {
        content = readFileToString(argv[1]);
        theSocketMaster.instantiateConfig(content);

        std::cout << "Starting webserv!" << std::endl;
        LOG_ERROR(INFO, INTERNAL_ERROR, Start, "main");

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
    if (errorHandler.shouldShutdown())
    {
        errorHandler.logSystemError(CRITICAL, INTERNAL_ERROR, Crit);
        std::cerr << Crit << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server shutdown without errors" << std::endl;
    return (0);
}
