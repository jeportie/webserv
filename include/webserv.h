/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:50:22 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/07 22:14:57 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define PORT 8666
# define BUFFER_SIZE 1024
# define MAXEVENTS 10
# define MAX_CLIENTS 1024
# define MAX_QUEUE_SIZE 1000
# define CLIENT_TIMEOUT 60  // seconds
# define MAX_RETRIES 3      // maximum number of retries for operations

# include <iostream>
# include <string>
# include "../src/class/ErrorHandler.hpp"

// Macro for easy error logging
# define LOG_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logError(level, category, message, source)

# define LOG_SYSTEM_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logSystemError(level, category, message, source)

# define THROW_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logError(level, category, message, source, true)

# define THROW_SYSTEM_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logSystemError(level, category, message, source, true)

#endif
