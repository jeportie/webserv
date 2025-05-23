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

/* Private Methodes of ErrorHandler */
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
