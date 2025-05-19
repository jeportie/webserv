/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/14 13:47:46 by jeportie         ###   ########.fr       */
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
#include "SocketManager.hpp"
#include "Callback.hpp"
#include "ErrorHandler.hpp"
#include "../../include/webserv.h"


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
    // Clean up server sockets
    for (std::map<int, ServerSocket*>::iterator it = _serverSockets.begin();
         it != _serverSockets.end(); ++it)
    {
        delete it->second;
    }
    _serverSockets.clear();
    
    // Clean up client sockets
    for (std::map<int, ClientSocket*>::iterator it = _clientSockets.begin();
         it != _clientSockets.end(); ++it)
    {
        delete it->second;
    }
    _clientSockets.clear();
}

/**
 * @brief Initializes server connections based on configuration
 * 
 * Creates server sockets for each configured server, binds them to the specified
 * host:port combinations, and starts listening for connections. Then creates an
 * epoll instance and registers all server sockets with it.
 * 
 * @param configs Vector of server configurations
 */
void SocketManager::init_connect_with_config(const std::vector<ServerConfig>& configs)
{
    // Create a server socket for each configured server
    for (size_t i = 0; i < configs.size(); ++i)
    {
        const ServerConfig& config = configs[i];
        
        ServerSocket* serverSocket = new ServerSocket();
        if (!serverSocket->safeBind(config.port, config.host))
        {
            std::stringstream ss;
            ss << "Failed to bind server socket on " << config.host << ":" << config.port;
            LOG_ERROR(ERROR, SOCKET_ERROR, ss.str(), "SocketManager::init_connect_with_config");
            delete serverSocket;
            continue; // Try to bind other servers even if one fails
        }
        
        serverSocket->safeListen(10);
        int serverSocketFd = serverSocket->getFd();
        
        // Store the server socket and its configuration
        _serverSockets[serverSocketFd] = serverSocket;
        _serverConfigs[serverSocketFd] = config;
        
        std::cout << "Server listening on " << config.host << ":" << config.port << std::endl;
    }
    
    // If no servers were successfully bound, throw an error
    if (_serverSockets.empty())
    {
        THROW_ERROR(CRITICAL, SOCKET_ERROR, "Failed to bind any server sockets", 
                   "SocketManager::init_connect_with_config");
    }
    
    // Create epoll instance
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to create epoll instance", 
                          "SocketManager::init_connect_with_config");
    }
    
    // Register all server sockets with epoll
    for (std::map<int, ServerSocket*>::iterator it = _serverSockets.begin();
         it != _serverSockets.end(); ++it)
    {
        registerServerSocketToEpoll(epoll_fd, it->first);
    }
    
    // Start the event loop
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
 * @param timeout_ms Timeout in milliseconds, -1 for infinite
 */
void SocketManager::eventLoop(int epoll_fd, int timeout_ms)
{
	int				n;
	int				fd;
	uint32_t		ev;
	ClientSocket	*client;

    while (true)
    {
        // Process any expired timers
        processTimers();

        // Process any pending callbacks
        processDeferredCallbacks();
        
        // Calculate timeout for epoll_wait based on next timer expiration
        int wait_timeout = timeout_ms;
        if (!_timerQueue.empty())
        {
            time_t now = time(NULL);
            time_t next_expire = _timerQueue.top().getExpireTime();
            if (next_expire <= now)
            {
                wait_timeout = 0; // Process immediately
            }
            else
            {
                wait_timeout = (next_expire - now) * 1000; // Convert to milliseconds
                if (timeout_ms != -1 && wait_timeout > timeout_ms)
                {
                    wait_timeout = timeout_ms; // Use the smaller timeout
                }
            }
        }

        // If there are pending callbacks, don't wait too long
        if (_callbackManager.hasPendingCallbacks() && (wait_timeout == -1 || wait_timeout > 100))
        {
            wait_timeout = 100; // Check for callbacks at least every 100ms
        }

        int n = epoll_wait(epoll_fd, events.data(), MAXEVENTS, wait_timeout);
        if (n < 0)
		{
            if (errno == EINTR)
                continue;
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", "SocketManager::eventLoop");
		}
        
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
    
            // Check if this is a server socket
            std::map<int, ServerSocket*>::iterator server_it = _serverSockets.find(fd);
            if ((ev & EPOLLIN) && server_it != _serverSockets.end())
            {
                // New connection on the server socket
                try
                {
                    // Get the server socket from the map
                    ServerSocket* serverSocket = _serverSockets[fd];
                    ClientSocket* client = serverSocket->safeAccept(epoll_fd);
                    if (client)
                    {
                        _clientSocketFd = client->getFd();
                        _clientSockets[_clientSocketFd] = client;
                        // Add a timeout for idle connections (60 seconds)
                        // We need to use a static function for C++98 compatibility
                        Callback* timeoutCallback = new Callback(&SocketManager::handleTimeout, _clientSocketFd, this, "timeout_handler");
                        addTimer(60, timeoutCallback);
                        // Schedule a callback to log the new connection
                        Callback* logCallback = new Callback(&SocketManager::logNewConnection, _clientSocketFd, client, "connection_logger");
                        executeDeferred(logCallback, CallbackManager::HIGH);
                    }
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR(ERROR, SOCKET_ERROR, "Accept failed: " + std::string(e.what()), "SocketManager::eventLoop");
                }
            }
            else if ((ev & EPOLLIN) && fd != _serverSocketFd)
            {
                // Data available on a client socket
                communication(fd);
                // Reset the timeout for this client
                cancelTimer(fd);
                Callback* timeoutCallback = new Callback(&SocketManager::handleTimeout, fd, this);
                addTimer(60, timeoutCallback);
            }
            else if (ev & (EPOLLERR | EPOLLHUP))
            {
                // Error or hangup on a socket
                std::cout << "Client disconnected (fd=" << fd << ")" << std::endl;
                cleanupClientSocket(fd, epoll_fd);
            }
        }
    }
}

/**
 * @brief Static callback function for logging new connections
 * 
 * @param fd The file descriptor of the new connection
 * @param data Pointer to the ClientSocket instance
 */
void SocketManager::logNewConnection(int fd, void* data)
{
    ClientSocket* client = static_cast<ClientSocket*>(data);
    std::cout << "New connection from " << client->getClientIP() 
              << ":" << client->getClientPort() 
              << " (fd=" << fd << ")" << std::endl;
}

/**
 * @brief Adds a callback to be executed immediately
 * 
 * @param callback The callback to execute
 */
void SocketManager::executeImmediate(Callback* callback)
{
    _callbackManager.executeImmediate(callback);
}

/**
 * @brief Adds a callback to be executed later
 * 
 * @param callback The callback to execute
 * @param priority The priority of the callback
 */
void SocketManager::executeDeferred(Callback* callback, CallbackManager::Priority priority)
{
    _callbackManager.executeDeferred(callback, priority);
}

/**
 * @brief Processes all deferred callbacks
 * 
 * Executes all deferred callbacks in priority order.
 */
void SocketManager::processDeferredCallbacks()
{
    _callbackManager.processDeferredCallbacks();
}

/**
 * @brief Cancels all callbacks for a specific file descriptor
 * 
 * @param fd The file descriptor to cancel callbacks for
 * @return int Number of callbacks cancelled
 */
int SocketManager::cancelCallbacksForFd(int fd)
{
    return _callbackManager.cancelCallbacksForFd(fd);
}

/**
 * @brief Cleans up resources for a client socket
 * 
 * @param fd The client socket file descriptor
 * @param epoll_fd The epoll file descriptor
 */
void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    // Cancel any timers for this client
    cancelTimer(fd);
    
    // Cancel any callbacks for this client
    cancelCallbacksForFd(fd);
    
    // Clean up the client socket
    if (fd != _serverSocketFd)
    {
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        // Remove from epoll
        if (epoll_fd >= 0)
        {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        }
        close(fd);
    }
}

/**
 * @brief Static callback function for handling timeouts
 * 
 * @param fd The file descriptor that timed out
 * @param data Pointer to the SocketManager instance
 */
void SocketManager::handleTimeout(int fd, void* data)
{
    SocketManager* manager = static_cast<SocketManager*>(data);
    std::cout << "Client connection timed out (fd=" << fd << ")" << std::endl;
    
    // Find the epoll_fd from the manager
    // This is a bit of a hack, but we need to get the epoll_fd somehow
    int epoll_fd = -1; // In a real implementation, you'd store this in the manager
    
    manager->cleanupClientSocket(fd, epoll_fd);
}

/**
 * @brief Adds a timer to the timer queue
 * 
 * @param seconds Seconds from now when the timer should expire
 * @param callback The callback to execute when the timer expires
 * @return int Timer ID
 */
int SocketManager::addTimer(int seconds, Callback* callback)
{
    time_t expireTime = time(NULL) + seconds;
    Timer timer(expireTime, callback);
    _timerQueue.push(timer);
    return callback->getFd(); // Use the file descriptor as the timer ID
}

/**
 * @brief Cancels a timer
 * 
 * @param fd The file descriptor associated with the timer
 * @return bool True if the timer was found and cancelled
 */
bool SocketManager::cancelTimer(int fd)
{
    // We can't easily remove from a priority queue, so we'll mark the callback as cancelled
    std::priority_queue<Timer> temp;
    bool found = false;
    
    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();
        _timerQueue.pop();
        
        Callback* callback = timer.getCallback();
        if (callback->getFd() == fd)
        {
            callback->cancel();
            found = true;
        }
        
        temp.push(timer);
    }
    
    _timerQueue = temp;
	return (found);
}

/**
 * @brief Processes expired timers
 * 
 * Checks for expired timers and executes their callbacks.
 */
void SocketManager::processTimers()
{
    time_t now = time(NULL);
    
    while (!_timerQueue.empty())
    {
        Timer timer = _timerQueue.top();
        
        if (timer.getExpireTime() > now)
        {
            // No more expired timers
            break;
        }
        
        // Remove the timer from the queue
        _timerQueue.pop();
        
        // Execute the callback if it's not cancelled
        Callback* callback = timer.getCallback();
        if (!callback->isCancelled())
        {
            callback->execute();
        }
        // Callback is owned by the Timer, which will be destroyed when it goes out of scope
    }
}

//     bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
//     if (bytes_read > 0)
//     {
//         buffer[bytes_read] = '\0';
//         std::cout << "Received from client (fd=" << fd << "): " << buffer;

    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        std::cout << "Received from client (fd=" << fd << "): " << buffer;
        
        // Echo back to the client
        if (write(fd, buffer, bytes_read) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, SOCKET_ERROR, "Failed to write response to client", "SocketManager::communication");
        }
    }
    else if (bytes_read == 0)
    {
        // Client closed the connection
        std::stringstream ss;
        ss << fd;
        LOG_ERROR(INFO, SOCKET_ERROR, "Client disconnected (fd=" + ss.str() + ")", "SocketManager::communication");
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        if (epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, EPOLL_ERROR, "Failed to remove client from epoll", "SocketManager::communication");
        }
        close(fd);
    }
    else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        // Unexpected read error
        std::stringstream ss;
        ss << fd;
        LOG_SYSTEM_ERROR(ERROR, SOCKET_ERROR, "Read error on fd " + ss.str(), "SocketManager::communication");
        
        // Clean up the client socket
        std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
        if (it != _clientSockets.end())
        {
            delete it->second;
            _clientSockets.erase(it);
        }
        
        // Remove from epoll
        if (epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            LOG_SYSTEM_ERROR(WARNING, EPOLL_ERROR, "Failed to remove client from epoll", "SocketManager::communication");
        }
        close(fd);
    }
    // If errno == EAGAIN or EWOULDBLOCK, we'll wait for the next EPOLLIN event
}

/**
 * @brief Sets the server socket to non-blocking mode
 *
 * @param fd The file descriptor to set to non-blocking mode
 * @return int Return value from fcntl call, -1 on error
 */
int SocketManager::setNonBlockingServer(int fd)
{
    // Find the server socket in the map
    std::map<int, ServerSocket*>::iterator it = _serverSockets.find(fd);
    if (it != _serverSockets.end()) {
        return it->second->setNonBlocking(fd);
    }
    return -1;
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
        std::stringstream ss;
        ss << "epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd << ", op=" << op << ")";
        LOG_SYSTEM_ERROR(ERROR, EPOLL_ERROR, ss.str(),
                        "SocketManager::safeEpollCtlClient");
        return -1;
    }
    return 0;
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
		if (needed > MAX_BODY_SIZE)
			throw HttpException(413, "Payload Too Large");
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
	HttpParser::parsePathAndQuerry(req.path, req.raw_query);
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


    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "Failed to add server socket to epoll", "SocketManager::safeRegisterToEpoll");
}

/**
 * @brief Registers a server socket with epoll
 * 
 * @param epoll_fd The epoll file descriptor
 * @param server_socket_fd The server socket file descriptor to register
 */
void SocketManager::registerServerSocketToEpoll(int epoll_fd, int server_socket_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_socket_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &ev) == -1)
    {
        std::stringstream ss;
        ss << "Failed to add server socket (fd=" << server_socket_fd << ") to epoll";
        THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, ss.str(), "SocketManager::registerServerSocketToEpoll");
    }
}


int SocketManager::getServerSocket(void) const { return _serverSocketFd; }

