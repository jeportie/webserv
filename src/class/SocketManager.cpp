/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 12:51:38 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/webserv.h"
#include "SocketManager.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <unistd.h>
#include <vector>
// #include "SocketManager.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "RequestLine.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "HttpException.hpp"
#include "HttpLimits.hpp"


SocketManager::SocketManager(void) : _serverSocketFd(-1), _clientSocketFd(-1)
{
}

SocketManager::~SocketManager(void)
{
	// Clean up client sockets
	for (std::map<int,
		ClientSocket *>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); ++it)
	{
		delete it->second;
	}
	_clientSockets.clear();
}

/**
 * @brief Initializes the server connection
 *
 * Creates a server socket, binds it to a port, and starts listening
 * for connections. Then creates an epoll instance and registers the
 * server socket with it.
 */
void SocketManager::init_connect(void)
{
	int	epoll_fd;

	// Create, bind, and listen on the server socket
	if (!_serverSocket.safeBind(PORT, ""))
		throw std::runtime_error("Failed to bind server socket");
	_serverSocket.safeListen(10);
	_serverSocketFd = _serverSocket.getFd();
	epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
		throw std::runtime_error("Failed to create epoll instance: "
			+ std::string(strerror(errno)));
	safeRegisterToEpoll(epoll_fd);
	std::cout << "Server listening on port " << PORT << std::endl;
	eventLoop(epoll_fd);
}

/**
 * @brief Runs the event loop
 *
 * Waits for events on the registered file descriptors and handles them.
 *
 * Note: Added a try-catch block around the safeAccept call.
 * This is to catch any exceptions thrown by the ServerSocket's safeAccept
 * method and log them, but continue the event loop. This is different from
 * the main() error handling because we want the server to continue running
 * even if one client connection fails.
 *
 * @param epoll_fd The epoll file descriptor
 */
void SocketManager::eventLoop(int epoll_fd)
{
	int				n;
	int				fd;
	uint32_t		ev;
	ClientSocket	*client;

	std::vector<epoll_event> events(MAXEVENTS);
	while (true)
	{
		n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, -1);
		if (n < 0)
			throw std::runtime_error("epoll_wait failed: "
				+ std::string(strerror(errno)));
		for (int i = 0; i < n; ++i)
		{
			fd = events[i].data.fd;
			ev = events[i].events;
			if ((ev & EPOLLIN) && fd == _serverSocketFd)
			{
				// New connection on the server socket
				try
				{
					client = _serverSocket.safeAccept(epoll_fd);
					if (client)
					{
						_clientSocketFd = client->getFd();
						_clientSockets[_clientSocketFd] = client;
					}
				}
				catch (const std::exception &e)
				{
					std::cerr << "Accept failed: " << e.what() << std::endl;
				}
			}
			else if ((ev & EPOLLIN) && fd != _serverSocketFd)
			{
				// Data available on a client socket
				communication(fd);
			}
			else if (ev & (EPOLLERR | EPOLLHUP))
			{
				// Error or hangup on a socket
				std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;
				// Clean up the client socket
				if (fd != _serverSocketFd)
				{
					std::map<int,
						ClientSocket *>::iterator it = _clientSockets.find(fd);
					if (it != _clientSockets.end())
					{
						delete it->second;
						_clientSockets.erase(it);
					}
					// Remove from epoll
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
				}
			}
		}
	}
}

// /**
//  * @brief Handles communication with a client
//  *
//  * Reads data from a client socket and processes it.
//  *
//  * @param fd The client socket file descriptor
//  */
// void SocketManager::communication(int fd)
// {
//     char buffer[BUFFER_SIZE];
//     int bytes_read;

//     bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         std::cout << "Received from client (fd=" << fd << "): " << buffer;

//         // Echo back to the client
//         write(fd, buffer, bytes_read);
//     }
//     else if (bytes_read == 0)
//     {
//         // Client closed the connection
//         std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;

//         // Clean up the client socket
//         std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
//         if (it != _clientSockets.end())
//         {
//             delete it->second;
//             _clientSockets.erase(it);
//         }

//         // Remove from epoll
//         epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
//         close(fd);
//     }
//     else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
//     {
//         // Unexpected read error
//         std::cerr << "Read error on fd " << fd << ": " << strerror(errno) << std::endl;

//         // Clean up the client socket
//         std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
//         if (it != _clientSockets.end())
//         {
//             delete it->second;
//             _clientSockets.erase(it);
//         }

//         // Remove from epoll
//         epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);
//         close(fd);
//     }
//     // If errno == EAGAIN or EWOULDBLOCK,
//	we'll wait for the next EPOLLIN event
// }

// If errno == EAGAIN or EWOULDBLOCK, we'll wait for the next EPOLLIN event

// **Explanation of Communication Changes:**

// 1. **Removed while loop**: The original code used a while loop to read all available data. I changed this to a single read call because with epoll in edge-triggered mode (EPOLLET),
//	we should read all available data at once.
//
// 2. **Added echo functionality**: Added code to echo back the received data to the client. This is a common feature in simple servers and helps demonstrate that the server is working correctly.
//
// 3. **Added client socket cleanup**: Added code to clean up the client socket when the client disconnects or there's an error. This includes removing it from the _clientSockets map.
//
// 4. **Improved logging**: Added more detailed log messages to show which client is sending data or disconnecting.
//
// 5. **Fixed epoll_ctl call**: The original code was missing the epoll_fd parameter in the epoll_ctl call when removing a client socket from epoll.

// … init_connect(), eventLoop(), closeConnection() déjà en place …

/**
 * @brief Sets the server socket to non-blocking mode
 *
 * @param fd The file descriptor to set to non-blocking mode
 * @return int Return value from fcntl call, -1 on error
 */
int SocketManager::setNonBlockingServer(int fd)
{
	return (_serverSocket.setNonBlocking(fd));
}

/**
 * @brief Safely registers a file descriptor with epoll
 *
 * @param epoll_fd The epoll file descriptor
 * @param op The operation to perform (EPOLL_CTL_ADD, EPOLL_CTL_MOD,
	EPOLL_CTL_DEL)
	* @param fd The file descriptor to register
	* @param event The epoll_event structure
	* @return int 0 on success, -1 on error
	*/
int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd,
	struct epoll_event *event)
{
	if (epoll_ctl(epoll_fd, op, fd, event) < 0)
	{
		std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd << ",op=" << op << "): " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

/**
 * @brief Registers the server socket with epoll
 *
 * @param epoll_fd The epoll file descriptor
 */
void SocketManager::safeRegisterToEpoll(int epoll_fd)
{
	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = _serverSocketFd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
		throw std::runtime_error("Failed to add server socket to epoll: "
			+ std::string(strerror(errno)));
}

int SocketManager::getServerSocket(void) const
{
	return (_serverSocketFd);
}

int SocketManager::getClientSocket(void) const
{
	return (_clientSocketFd);
}


bool SocketManager::readFromClient(int fd)
{
	ClientSocket* client = _clientSockets[fd];
    std::string&   buf    = client->getBuffer();
    char           tmp[4096];
	
    while (true) {
		ssize_t n = ::read(fd, tmp, sizeof(tmp));
        if (n > 0) {
			buf.append(tmp, n);
            continue;
        }
        // n <= 0 : soit EOF (n==0), soit plus de données pour l'instant (n<0/EAGAIN)
        break;
    }
    return true;
}



// 2) cherche et parse les headers si pas déjà fait

bool SocketManager::parseClientHeaders(ClientSocket* client)
{
	if (client->headersParsed())
	return true;
	
    std::string& buf = client->getBuffer();
    size_t hdr_end = buf.find("\r\n\r\n");
    if (hdr_end == std::string::npos)
	return false;
	
    std::string hdr_block = buf.substr(0, hdr_end);
    size_t line_end = hdr_block.find("\r\n");
    std::string firstLine = hdr_block.substr(0, line_end);
	
    // Request-Line
	std::cout << firstLine << std::endl;
    RequestLine rl = HttpParser::parseRequestLine(firstLine);
	client->setRequestLine(rl);

    // Headers
    std::string rest = hdr_block.substr(line_end + 2);
    std::map<std::string, std::vector<std::string> > hdrs = HttpParser::parseHeaders(rest);
	if (hdrs.size() > MAX_HEADER_COUNT)
    	throw HttpException(431, "Request Header Fields Too Large");
    client->setParsedHeaders(hdrs);
    client->setHeadersParsed(true);
    buf.erase(0, hdr_end + 4);
	
    // Délégation au client pour détermination du mode de body
    client->determineBodyMode();
    return true;
}



static size_t hexToSize(const std::string& hex)
{
    size_t result = 0;
    std::istringstream iss(hex);
    iss >> std::hex >> result;
	if (result == SIZE_MAX)
    	throw HttpException(400, "Bad Request");                     // taille invalide

	if (result > MAX_CHUNK_SIZE)
    	throw HttpException(413, "Payload Too Large");               // chunk trop gros
    return result;
}

bool SocketManager::parseClientBody(ClientSocket* client)
{
	std::string& buf = client->getBuffer();
    BodyMode mode = client->getBodyMode();
	
    if (mode == BODY_CONTENT_LENGTH) {
		size_t needed = client->getContentLength();
        if (buf.size() < needed) return false;
        // leave body in buffer until buildHttpRequest
        return true;
    } else if (mode == BODY_CHUNKED) {
		// chunked state machine, append to internal temporary storage
        while (true) {
			if (client->getChunkSize() == 0) {
				size_t pos = buf.find("\r\n");
                if (pos == std::string::npos) return false;
                size_t chunkLen = hexToSize(buf.substr(0, pos));
				if (buf.size() + chunkLen > MAX_BODY_SIZE)
    				throw HttpException(413, "Payload Too Large");
                client->setChunkSize(chunkLen);
                buf.erase(0, pos + 2);
                if (chunkLen == 0) {
					size_t end = buf.find("\r\n");
                    if (end == std::string::npos) return false;
                    buf.erase(0, end + 2);
                    break;
                }
            }
            size_t needed = client->getChunkSize();
            if (buf.size() < needed + 2) return false;
            client->getBodyAccumulator().append(buf.substr(0, needed));
            buf.erase(0, needed + 2);
            client->setChunkSize(0);
        }
        return true;
    }
    // No body
    return true;
}

HttpRequest SocketManager::buildHttpRequest(ClientSocket* client)
{
	HttpRequest req;
    // fill body
    if (client->getBodyMode() == BODY_CONTENT_LENGTH) {
		req.body = client->getBuffer().substr(0, client->getContentLength());
        client->getBuffer().erase(0, client->getContentLength());
    } else if (client->getBodyMode() == BODY_CHUNKED) {
		req.body = client->getBodyAccumulator();
        client->clearBodyAccumulator();
    }
	
    // fill request-line and headers
    RequestLine rl = client->getRequestLine();
    req.method     = rl.method;
    req.http_major = rl.http_major;
    req.http_minor = rl.http_minor;
    HttpParser::splitTarget(rl.target, req.path, req.raw_query);
    req.headers      = client->getParsedHeaders();
    req.query_params = HttpParser::parseQueryParams(req.raw_query);
    req.form_data    = HttpParser::parseFormUrlencoded(req.body);
    return req;
}

void SocketManager::cleanupRequest(ClientSocket* client)
{
	client->resetParserState();
}

/**
 * @brief Closes a client connection, removing it from epoll and cleaning up resources
 *
 * @param fd The client socket file descriptor to close
 * @param epoll_fd The epoll instance file descriptor; if >=0, the socket will be deregistered
 */
void SocketManager::closeConnection(int fd, int epoll_fd)
{
    // 1) Deregister from epoll if applicable
    if (epoll_fd >= 0) {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            // Log error but continue cleanup
            std::cerr << "epoll_ctl DEL failed for fd " << fd
                      << ": " << strerror(errno) << std::endl;
        }
    }

    // 2) Delete the ClientSocket object
    std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
    if (it != _clientSockets.end()) {
        delete it->second;
        _clientSockets.erase(it);
    }

    // 3) Close the file descriptor
    if (close(fd) == -1) {
        std::cerr << "close() failed for fd " << fd
                  << ": " << strerror(errno) << std::endl;
    }
}

bool SocketManager::communication(int fd)
{
	ClientSocket* client = _clientSockets[fd];

	// 1) Lire toutes les données disponibles
	try
	{
		
		if (!readFromClient(fd))
		return false;
		
		// 2) Parser les en-têtes si ce n'est pas déjà fait
		if (!parseClientHeaders(client))
		return false;
		
		// 3) Parser ou accumuler le corps (mode Content-Length ou chunked)
		if (!parseClientBody(client))
		return false;
		
		// 4) Construire l'objet HttpRequest complet
		HttpRequest req = buildHttpRequest(client);
		
		// 5) Passer la requête au handler
		// handleHttpRequest(fd, req);
		
		// 6) Nettoyer pour la requête suivante (pipeline ou fermeture)
		cleanupRequest(client);
		// Si le client a demandé explicitement la fermeture…
		if (req.headers.count("Connection") &&
		req.headers["Connection"][0] == "close")
		{
			return false;  // plus de traitement sur cette socket
			// closeConnection(fd, epoll_fd); REPLACER DASN EVENT LOOOP
		}
	}
	catch(const HttpException& he)
	{
		  // Erreur de la requête (4xx / 5xx) :
        sendErrorResponse(fd, he.status(), he.what());
        // closeConnection(fd, epoll_fd);       // retire de epoll, delete ClientSocket, close(fd)
        return false;                        // on arrête là pour ce fd
	}
    return true;  // on garde la connexion ouverte
}



