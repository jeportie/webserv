/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 15:30:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 15:30:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <cstring>
# include <cerrno>
# include <ctime>
# include <fstream>
# include <map>

/**
 * @brief Error severity levels
 */
enum ErrorLevel {
    DEBUG,      // Detailed information for debugging
    INFO,       // General information about normal operation
    WARNING,    // Potential issues that don't prevent operation
    ERROR,      // Errors that affect operation but don't require shutdown
    CRITICAL    // Critical errors that require immediate shutdown
};

/**
 * @brief Error categories for classification
 */
enum ErrorCategory {
    SOCKET_ERROR,       // Socket-related errors
    EPOLL_ERROR,        // Epoll-related errors
    CALLBACK_ERROR,     // Callback-related errors
    TIMER_ERROR,        // Timer-related errors
    RESOURCE_ERROR,     // Resource exhaustion errors
    SYSTEM_ERROR,       // System call errors
    INTERNAL_ERROR      // Internal logic errors
};

/**
 * @brief Singleton class for centralized error handling
 * 
 * This class provides a centralized system for error handling, logging,
 * and recovery strategies. It uses a singleton pattern to ensure a single
 * instance throughout the application.
 */
class ErrorHandler
{
public:
    /**
     * @brief Get the singleton instance
     * 
     * @return ErrorHandler& Reference to the singleton instance
     */
    static ErrorHandler& getInstance();

    /**
     * @brief Log an error with context information
     * 
     * @param level Error severity level
     * @param category Error category
     * @param message Error message
     * @param source Source of the error (function, class, etc.)
     * @param shouldThrow Whether to throw an exception after logging
     */
    void logError(ErrorLevel level, ErrorCategory category, const std::string& message, 
                  const std::string& source = "", bool shouldThrow = false);

    /**
     * @brief Log a system error (using errno)
     * 
     * @param level Error severity level
     * @param category Error category
     * @param message Error message prefix
     * @param source Source of the error (function, class, etc.)
     * @param shouldThrow Whether to throw an exception after logging
     */
    void logSystemError(ErrorLevel level, ErrorCategory category, const std::string& message, 
                        const std::string& source = "", bool shouldThrow = false);

    /**
     * @brief Set the minimum log level
     * 
     * @param level Minimum level to log
     */
    void setLogLevel(ErrorLevel level);

    /**
     * @brief Set the log file path
     * 
     * @param path Path to the log file
     * @return bool True if successful, false otherwise
     */
    bool setLogFile(const std::string& path);

    /**
     * @brief Check if the application should shut down due to critical errors
     * 
     * @return bool True if shutdown is recommended
     */
    bool shouldShutdown() const;

    /**
     * @brief Reset the shutdown flag
     */
    void resetShutdown();

    /**
     * @brief Get error statistics
     * 
     * @return std::map<ErrorCategory, int> Map of error counts by category
     */
    std::map<ErrorCategory, int> getErrorStats() const;

    /**
     * @brief Reset error statistics
     */
    void resetErrorStats();

private:
    ErrorHandler();
    ~ErrorHandler();
    ErrorHandler(const ErrorHandler&);
    ErrorHandler& operator=(const ErrorHandler&);

    /**
     * @brief Format a timestamp for logging
     * 
     * @return std::string Formatted timestamp
     */
    std::string getTimestamp() const;

    /**
     * @brief Convert error level to string
     * 
     * @param level Error level
     * @return std::string String representation
     */
    std::string levelToString(ErrorLevel level) const;

    /**
     * @brief Convert error category to string
     * 
     * @param category Error category
     * @return std::string String representation
     */
    std::string categoryToString(ErrorCategory category) const;

    ErrorLevel _minLogLevel;                  // Minimum level to log
    std::ofstream _logFile;                   // Log file stream
    bool _shouldShutdown;                     // Shutdown flag
    std::map<ErrorCategory, int> _errorStats; // Error statistics
};

#endif  // ************************************************* ERRORHANDLER_HPP //
