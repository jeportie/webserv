/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 15:30:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/26 12:10:21 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <cstring>
#include <cerrno>
#include <ctime>
#include <fstream>

#include <map>

#define LOG_ERR_CONST "ErrorHandler constructor Called"
#define LOG_ERR_DEST "ErrorHandler destructor Called"

// Error handling enums
enum ErrorLevel
{
    DEBUG,    // Detailed information for debugging
    INFO,     // General information about normal operation
    WARNING,  // Potential issues that don't prevent operation
    ERROR,    // Errors that affect operation but don't require shutdown
    CRITICAL  // Critical errors that require immediate shutdown
};

enum ErrorCategory
{
    SOCKET_ERROR,    // Socket-related errors
    EPOLL_ERROR,     // Epoll-related errors
    CALLBACK_ERROR,  // Callback-related errors
    TIMER_ERROR,     // Timer-related errors
    RESOURCE_ERROR,  // Resource exhaustion errors
    SYSTEM_ERROR,    // System call errors
    INTERNAL_ERROR,  // Internal logic errors
    HTTP_REQ_ERROR,  // Http Request errors
    CONFIG_FILE_ERROR // Erreur dans le fichier config
};

typedef std::map<ErrorCategory, int> ERRMAP;

class ErrorHandler
{
public:
    static ErrorHandler& getInstance();

    void logError(ErrorLevel				level,
                  ErrorCategory				category,
                  const std::string&		message,
                  const std::string&		source      = "",
                  bool						shouldThrow = false);

    void logSystemError(ErrorLevel			level,
                        ErrorCategory		category,
                        const std::string&	message,
                        const std::string&	source      = "",
                        bool				shouldThrow = false);

    void			setLogLevel(ErrorLevel level);
    bool			setLogFile(const std::string& path);
    bool			shouldShutdown() const;
    void			resetShutdown();

    ERRMAP			getErrorStats() const;

    void			resetErrorStats();

private:
    ErrorHandler();
    ~ErrorHandler();
    ErrorHandler(const ErrorHandler&);

    ErrorHandler&	operator=(const ErrorHandler&);

    std::string		getTimestamp() const;
    std::string		levelToString(ErrorLevel level) const;
    std::string		categoryToString(ErrorCategory category) const;

    ErrorLevel      _minLogLevel;     // Minimum level to log
    std::ofstream   _logFile;         // Log file stream
    bool            _shouldShutdown;  // Shutdown flag
    ERRMAP			_errorStats;      // Error statistics
};

#endif  // ************************************************* ERRORHANDLER_HPP //
