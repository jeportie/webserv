/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketLogMessages.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: AI                                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 12:30:00 by AI                #+#    #+#             */
/*   Updated: 2025/05/24 12:30:00 by AI               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETLOGMESSAGES_HPP
#define SOCKETLOGMESSAGES_HPP

#define LOG_SOCKET_CREATE_FAIL "Failed to create socket"
#define LOG_FCNTL_SERVER_FAIL "fcntl server socket failed"
#define LOG_BIND_FAIL "Bind failed"
#define LOG_LISTEN_FAIL "Listen() Failed"
#define LOG_INVALID_SOCKET_LISTEN "Cannot listen on invalid socket"
#define LOG_INVALID_SOCKET_ACCEPT "Cannot accept on invalid socket"
#define LOG_ACCEPT_FAIL "Accept() Failed"
#define LOG_NO_MORE_CLIENTS "No more Clients to Accept in the queue"
#define LOG_ACCEPTED_CONNECTION "Connection accepted from "
#define LOG_ADD_CLIENT_EPOLL_FAIL "Failed to add client to epoll"

// Additional log/output strings from all socket classes
#define LOG_CLIENTSOCKET_CONSTRUCTOR "ClientSocket Constructor called."
#define LOG_CLIENTSOCKET_DESTRUCTOR "ClientSocket Destructor called."
#define LOG_UPDATED_TIMESTAMP "Updated Timestamp of client fd="
#define LOG_FCNTL_FAILED "fcntl failed on client fd "
#define LOG_NOT_IMPLEMENTED "Not Implemented"
#define LOG_PAYLOAD_TOO_LARGE "Payload Too Large"
#define LOG_SERVERSOCKET_CONSTRUCTOR "ServerSocket Constructor called."
#define LOG_SERVERSOCKET_DESTRUCTOR "ServerSocket Destructor called."
#define LOG_SOCKET_CONSTRUCTOR "Socket Constructor called"
#define LOG_SOCKET_DESTRUCTOR "Socket Desstructor called"
#define LOG_SOCKET_COPY_CONSTRUCTOR "Socket Copy constructor called"
#define LOG_ERROR_CREATING_SOCKET "Error creating socket: "
#define LOG_CANNOT_SET_SO_REUSEADDR "Cannot set SO_REUSEADDR on invalid socket"
#define LOG_ERROR_SETTING_SO_REUSEADDR "Error setting SO_REUSEADDR: "

#endif // SOCKETLOGMESSAGES_HPP

