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

#define LOG_ERR_CONST "ErrorHandler constructor Called"
#define LOG_ERR_DEST "ErrorHandler destructor Called"

#include <cstring>
#include <cerrno>
#include <ctime>
#include <fstream>

#include "../../../include/webserv.h"

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
