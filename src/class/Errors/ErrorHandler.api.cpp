/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 15:30:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 15:30:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorHandler.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>

void ErrorHandler::setLogLevel(ErrorLevel level) { _minLogLevel = level; }

bool ErrorHandler::shouldShutdown() const { return _shouldShutdown; }

void ErrorHandler::resetShutdown() { _shouldShutdown = false; }

std::map<ErrorCategory, int> ErrorHandler::getErrorStats() const { return _errorStats; }

ErrorHandler& ErrorHandler::getInstance()
{
    static ErrorHandler instance;
    return instance;
}

bool ErrorHandler::setLogFile(const std::string& path)
{
    // Close existing log file if open
    if (_logFile.is_open())
        _logFile.close();

    // Open new log file
    _logFile.open(path.c_str(), std::ios::app);

    return _logFile.is_open();
}

void ErrorHandler::resetErrorStats()
{
	std::map<ErrorCategory, int>::iterator it;

    for (it = _errorStats.begin(); it != _errorStats.end(); ++it)
        it->second = 0;
}

void ErrorHandler::logError(ErrorLevel         level,
                            ErrorCategory      category,
                            const std::string& message,
                            const std::string& source,
                            bool               shouldThrow)
{
    // Increment error count for this category
    _errorStats[category]++;
    // Check if we should log this level
    if (level < _minLogLevel)
        return;
    // Format the log message
    std::ostringstream logStream;
    logStream << getTimestamp() << " [" << levelToString(level) << "] "
              << "[" << categoryToString(category) << "] ";
    if (!source.empty())
        logStream << "[" << source << "] ";
    logStream << message;
    // Write to log file if open
    if (_logFile.is_open())
    {
        _logFile << logStream.str() << std::endl;
        _logFile.flush();
    }
    // Also write to stderr for ERROR and CRITICAL
    if (level >= ERROR)
    {
        std::cerr << logStream.str() << std::endl;
    }
    // Set shutdown flag for CRITICAL errors
    if (level == CRITICAL)
    {
        _shouldShutdown = true;
    }
    // Throw exception if requested
    if (shouldThrow)
    {
        throw std::runtime_error(logStream.str());
    }
}

void ErrorHandler::logSystemError(ErrorLevel         level,
                                  ErrorCategory      category,
                                  const std::string& message,
                                  const std::string& source,
                                  bool               shouldThrow)
{
    std::string errorMsg = message + ": " + std::string(strerror(errno));
    logError(level, category, errorMsg, source, shouldThrow);
}
