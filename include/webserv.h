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
#define WEBSERV_HPP

#define PORT 8666
#define BUFFER_SIZE 1024
#define MAXEVENTS 10
#define MAX_CLIENTS 1024
#define MAX_QUEUE_SIZE 1000
#define CLIENT_TIMEOUT 60  // seconds
#define MAX_RETRIES 3      // maximum number of retries for operations

#include <sys/epoll.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

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

// Macro for easy error logging
#define LOG_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logError(level, category, message, source)

#define LOG_SYSTEM_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logSystemError(level, category, message, source)

#define THROW_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logError(level, category, message, source, true)

#define THROW_SYSTEM_ERROR(level, category, message, source) \
    ErrorHandler::getInstance().logSystemError(level, category, message, source, true)

/* Typedef for all complexe types of Webserv project */

// Forward declarations
class ServerConfig;
class RouteConfig;
class ServerSocket;
class ClientSocket;

// Map 
typedef std::map<int, std::string>							ISMAP;
typedef std::map<int, ClientSocket*>						ICMAP;
typedef std::map<ErrorCategory, int>						ERRMAP;
typedef std::map<std::string, RouteConfig>					ROUTEMAP;
typedef std::map<std::string, std::string>					SSMAP;
typedef std::map<int, std::vector<ServerConfig> >			IVSCMAP;
typedef std::map<std::string, std::vector<std::string> >	SVSMAP;

// Pair
typedef std::pair<std::string, std::string>					SSPAIR;

// Vector
typedef std::vector<std::string>							SVECTOR;
typedef std::vector<epoll_event>							EVENT_LIST;
typedef std::vector<ServerSocket>							SSVECTOR;

#endif
