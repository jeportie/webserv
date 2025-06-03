/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:50:22 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 17:41:11 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#define BUFFER_SIZE 1024

#define MAXEVENTS 10
#define MAX_CLIENTS 1024
#define MAX_QUEUE_SIZE 1000
#define CLIENT_TIMEOUT 60  // seconds
#define MAX_RETRIES 3      // maximum number of retries for operations

#include <sys/epoll.h>
#include <string>
#include <vector>
#include <map>

// Macro for easy error logging
#define LOG_ERROR(level, category, message, source)            \
    ErrorHandler::getInstance().logError(level, category, message, source)

#define LOG_SYSTEM_ERROR(level, category, message, source)     \
    ErrorHandler::getInstance().logSystemError(level, category, message, source)

#define THROW_ERROR(level, category, message, source)          \
    ErrorHandler::getInstance().logError(level, category, message, source, true)

#define THROW_SYSTEM_ERROR(level, category, message, source)   \
    ErrorHandler::getInstance().logSystemError(level, category, message, source, true)

/* Typedef for all complexe types of Webserv project */

// Map
typedef std::map<int, std::string>                      ISMAP;
typedef std::map<std::string, std::string>              SSMAP;
typedef std::map<std::string, std::vector<std::string> > SVSMAP;

// Pair
typedef std::pair<std::string, std::string> SSPAIR;

// Vector
typedef std::vector<std::string> SVECTOR;
typedef std::vector<epoll_event> EVENT_LIST;

std::string readFileToString(const std::string& filename);

#endif  // ****************************************************** WEBSERV_HPP //
