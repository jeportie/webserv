/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_error_handler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 16:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../src/class/ErrorHandler.hpp"
#include "../include/webserv.h"
#include <iostream>
#include <stdexcept>

// Test function to demonstrate error handling
void testErrorLevels()
{
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    std::cout << "Testing different error levels:" << std::endl;
    
    // Log messages at different levels
    errorHandler.logError(DEBUG, INTERNAL_ERROR, "This is a debug message", "testErrorLevels");
    errorHandler.logError(INFO, INTERNAL_ERROR, "This is an info message", "testErrorLevels");
    errorHandler.logError(WARNING, INTERNAL_ERROR, "This is a warning message", "testErrorLevels");
    errorHandler.logError(ERROR, INTERNAL_ERROR, "This is an error message", "testErrorLevels");
    
    // Don't log CRITICAL as it sets the shutdown flag
    
    std::cout << "All messages logged successfully" << std::endl;
}

// Test function to demonstrate system error handling
void testSystemErrors()
{
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    std::cout << "Testing system error handling:" << std::endl;
    
    // Set errno to a known value
    errno = EACCES; // Permission denied
    
    // Log a system error
    errorHandler.logSystemError(ERROR, SYSTEM_ERROR, "Failed to open file", "testSystemErrors");
    
    std::cout << "System error logged successfully" << std::endl;
}

// Test function to demonstrate error statistics
void testErrorStats()
{
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    std::cout << "Testing error statistics:" << std::endl;
    
    // Reset error statistics
    errorHandler.resetErrorStats();
    
    // Log some errors
    errorHandler.logError(WARNING, SOCKET_ERROR, "Socket warning 1", "testErrorStats");
    errorHandler.logError(WARNING, SOCKET_ERROR, "Socket warning 2", "testErrorStats");
    errorHandler.logError(ERROR, EPOLL_ERROR, "Epoll error", "testErrorStats");
    
    // Get error statistics
    std::map<ErrorCategory, int> stats = errorHandler.getErrorStats();
    
    // Print statistics
    std::cout << "Error statistics:" << std::endl;
    std::cout << "SOCKET_ERROR: " << stats[SOCKET_ERROR] << std::endl;
    std::cout << "EPOLL_ERROR: " << stats[EPOLL_ERROR] << std::endl;
    std::cout << "CALLBACK_ERROR: " << stats[CALLBACK_ERROR] << std::endl;
    
    // Reset statistics again
    errorHandler.resetErrorStats();
    
    std::cout << "Error statistics reset successfully" << std::endl;
}

// Test function to demonstrate exception throwing
void testExceptionThrowing()
{
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    std::cout << "Testing exception throwing:" << std::endl;
    
    try
    {
        // This should throw an exception
        errorHandler.logError(ERROR, INTERNAL_ERROR, "This will throw an exception", "testExceptionThrowing", true);
        
        // This line should not be reached
        std::cout << "ERROR: Exception was not thrown!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception caught successfully: " << e.what() << std::endl;
    }
}

// Test function to demonstrate the shutdown flag
void testShutdownFlag()
{
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    std::cout << "Testing shutdown flag:" << std::endl;
    
    // Reset the shutdown flag
    errorHandler.resetShutdown();
    
    // Check initial state
    std::cout << "Initial shutdown flag: " << (errorHandler.shouldShutdown() ? "true" : "false") << std::endl;
    
    // Log a critical error (sets the shutdown flag)
    errorHandler.logError(CRITICAL, INTERNAL_ERROR, "Critical error occurred", "testShutdownFlag");
    
    // Check final state
    std::cout << "Final shutdown flag: " << (errorHandler.shouldShutdown() ? "true" : "false") << std::endl;
    
    // Reset the shutdown flag again
    errorHandler.resetShutdown();
    
    std::cout << "Shutdown flag reset successfully" << std::endl;
}

int main()
{
    // Initialize error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    
    // Set log level and log file
    errorHandler.setLogLevel(DEBUG); // Set to DEBUG to see all messages
    errorHandler.setLogFile("error_handler_test.log");
    
    std::cout << "ErrorHandler Test Program" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Run tests
    testErrorLevels();
    std::cout << std::endl;
    
    testSystemErrors();
    std::cout << std::endl;
    
    testErrorStats();
    std::cout << std::endl;
    
    testExceptionThrowing();
    std::cout << std::endl;
    
    testShutdownFlag();
    std::cout << std::endl;
    
    std::cout << "All tests completed successfully" << std::endl;
    std::cout << "Check error_handler_test.log for logged messages" << std::endl;
    
    return 0;
}
