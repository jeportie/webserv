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
#include <iomanip>

ErrorHandler::ErrorHandler()
: _minLogLevel(INFO)
, _shouldShutdown(false)
{
    // Initialize error statistics
    _errorStats[SOCKET_ERROR]   = 0;
    _errorStats[EPOLL_ERROR]    = 0;
    _errorStats[CALLBACK_ERROR] = 0;
    _errorStats[TIMER_ERROR]    = 0;
    _errorStats[RESOURCE_ERROR] = 0;
    _errorStats[SYSTEM_ERROR]   = 0;
    _errorStats[INTERNAL_ERROR] = 0;
}

ErrorHandler::~ErrorHandler()
{
    if (_logFile.is_open())
        _logFile.close();
}

ErrorHandler& ErrorHandler::getInstance()
{
    static ErrorHandler instance;
    return instance;
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
    // Get the system error message
    std::string errorMsg = message + ": " + std::string(strerror(errno));

    // Log the error
    logError(level, category, errorMsg, source, shouldThrow);
}

void ErrorHandler::setLogLevel(ErrorLevel level) { _minLogLevel = level; }

bool ErrorHandler::setLogFile(const std::string& path)
{
    // Close existing log file if open
    if (_logFile.is_open())
        _logFile.close();

    // Open new log file
    _logFile.open(path.c_str(), std::ios::app);

    return _logFile.is_open();
}

bool ErrorHandler::shouldShutdown() const { return _shouldShutdown; }

void ErrorHandler::resetShutdown() { _shouldShutdown = false; }

std::map<ErrorCategory, int> ErrorHandler::getErrorStats() const { return _errorStats; }

void ErrorHandler::resetErrorStats()
{
    for (std::map<ErrorCategory, int>::iterator it = _errorStats.begin(); it != _errorStats.end();
         ++it)
    {
        it->second = 0;
    }
}

std::string ErrorHandler::getTimestamp() const
{
    time_t     now      = time(NULL);
    struct tm* timeinfo = localtime(&now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return std::string(buffer);
}

std::string ErrorHandler::levelToString(ErrorLevel level) const
{
    switch (level)
    {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

std::string ErrorHandler::categoryToString(ErrorCategory category) const
{
    switch (category)
    {
        case SOCKET_ERROR:
            return "SOCKET";
        case EPOLL_ERROR:
            return "EPOLL";
        case CALLBACK_ERROR:
            return "CALLBACK";
        case TIMER_ERROR:
            return "TIMER";
        case RESOURCE_ERROR:
            return "RESOURCE";
        case SYSTEM_ERROR:
            return "SYSTEM";
        case INTERNAL_ERROR:
            return "INTERNAL";
        default:
            return "UNKNOWN";
    }
}
