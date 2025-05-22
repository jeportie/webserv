/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 23:35:12 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/22 13:21:45 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sstream>

#include "SocketManager.hpp"
#include "../../../include/webserv.h"
#include "../Callbacks/AcceptCallback.hpp"
#include "../Callbacks/ErrorCallback.hpp"
#include "../Callbacks/ReadCallback.hpp"
#include "../Callbacks/TimeoutCallback.hpp"
#include "../Http/RequestLine.hpp"
#include "../Sockets/ClientSocket.hpp"

#include "../Http/HttpParser.hpp"
#include "../Http/HttpRequest.hpp"
#include "../Http/HttpException.hpp"
#include "../Http/HttpLimits.hpp"

void SocketManager::init_connect(void)
{
    // Create, bind, and listen on the server socket
    if (!_serverSocket.safeBind(PORT, ""))
        THROW_ERROR(
            CRITICAL, SOCKET_ERROR, "Failed to bind server socket", "SocketManager::init_connect");

    _serverSocket.safeListen(10);
    _serverSocketFd = _serverSocket.getFd();

    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
        THROW_SYSTEM_ERROR(CRITICAL,
                           EPOLL_ERROR,
                           "Failed to create epoll instance",
                           "SocketManager::init_connect");

    safeRegisterToEpoll(epoll_fd);
    std::cout << "Server listening on port " << PORT << std::endl;
    eventLoop(epoll_fd);
}

void SocketManager::eventLoop(int epoll_fd)
{
    std::vector<epoll_event> events;
    int checkIntervalMs;
    bool running;
    int n;

    events = std::vector<epoll_event>(MAXEVENTS);
    checkIntervalMs = getCheckIntervalMs();
    running = true;

    while (running)
    {
        /* 1) Wait up to CHECK_INTERVAL_MS for any I/O */
        n = epoll_wait(epoll_fd, &events[0], MAXEVENTS, checkIntervalMs);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            THROW_SYSTEM_ERROR(CRITICAL, EPOLL_ERROR, "epoll_wait failed", "SocketManager::eventLoop");
        }

        scanClientTimeouts(epoll_fd);

        _callbackQueue.processCallbacks();

        enqueueReadyCallbacks(n, events, epoll_fd);
    }
}

int SocketManager::getCheckIntervalMs(void)
{
    return 1000;
}

void SocketManager::enqueueReadyCallbacks(int n, std::vector<epoll_event>& events, int epoll_fd)
{
    int i;
    int fd;
    uint32_t ev;

    i = 0;
    while (i < n)
    {
        fd = events[i].data.fd;
        ev = events[i].events;

        if ((ev & EPOLLIN) && fd == _serverSocketFd)
        {
            _callbackQueue.push(new AcceptCallback(fd, this, epoll_fd));
        }
        else if ((ev & EPOLLIN) && fd != _serverSocketFd)
        {
            _callbackQueue.push(new ReadCallback(fd, this));
        }
        else if (ev & (EPOLLERR | EPOLLHUP))
        {
            _callbackQueue.push(new ErrorCallback(fd, this, epoll_fd));
        }
        i++;
    }
}

void SocketManager::scanClientTimeouts(int epoll_fd)
{
    time_t now;
    std::map<int, ClientSocket*>::iterator it;
    int fd;
    ClientSocket* c;
    std::ostringstream oss;

    now = time(NULL);
    it = _clientSockets.begin();
    while (it != _clientSockets.end())
    {
        fd = it->first;
        c = it->second;
        if (now - c->getLastActivity() >= CLIENT_TIMEOUT)
        {
            oss.str("");
            oss.clear();
            oss << "Client timed out (fd=" << fd << ")";
            LOG_ERROR(INFO, SOCKET_ERROR,
                      oss.str(),
                      "SocketManager::scanClientTimeouts");
            _callbackQueue.push(new TimeoutCallback(fd, this, epoll_fd));
        }
        ++it;
    }
}


void SocketManager::addClientSocket(int fd, ClientSocket* client) { _clientSockets[fd] = client; }

ServerSocket& SocketManager::getServerSocket() { return _serverSocket; }

CallbackQueue& SocketManager::getCallbackQueue() { return _callbackQueue; }


void SocketManager::cleanupClientSocket(int fd, int epoll_fd)
{
    // Remove from epoll first
    if (epoll_fd >= 0)
    {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    }

    // Then delete the client socket object
    std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        delete it->second;
        _clientSockets.erase(it);
    }

    // Finally close the file descriptor
    close(fd);
}


void SocketManager::safeRegisterToEpoll(int epoll_fd)
{
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = _serverSocketFd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverSocketFd, &ev) == -1)
        THROW_SYSTEM_ERROR(CRITICAL,
                           EPOLL_ERROR,
                           "Failed to add server socket to epoll",
                           "SocketManager::safeRegisterToEpoll");
}

int SocketManager::setNonBlockingServer(int fd) { return (_serverSocket.setNonBlocking(fd)); }

int SocketManager::safeEpollCtlClient(int epoll_fd, int op, int fd, struct epoll_event* event)
{
    if (epoll_ctl(epoll_fd, op, fd, event) < 0)
    {
        std::cerr << "[Error] epoll_ctl failed (epoll_fd=" << epoll_fd << ", fd=" << fd
                  << ",op=" << op << "): " << strerror(errno) << std::endl;
        return (-1);
    }
    return (0);
}

int SocketManager::getServerSocketFd(void) const { return (_serverSocketFd); }

int SocketManager::getClientSocketFd(void) const { return (_clientSocketFd); }


bool SocketManager::readFromClient(int fd)
{
    ClientSocket* client = _clientSockets[fd];
    std::string&  buf    = client->getBuffer();
    char          tmp[4096];

    while (true)
    {
        ssize_t n = ::read(fd, tmp, sizeof(tmp));
        if (n > 0)
        {
            buf.append(tmp, n);
			client->touch();
            continue;
        }
        // n <= 0 : soit EOF (n==0), soit plus de données pour l'instant (n<0/EAGAIN)
        break;
    }
    return true;
}

bool SocketManager::parseClientHeaders(ClientSocket* client)
{
    if (client->headersParsed())
        return true;

    std::string& buf     = client->getBuffer();
    size_t       hdr_end = buf.find("\r\n\r\n");
    if (hdr_end == std::string::npos)
        return false;

    std::string hdr_block = buf.substr(0, hdr_end);
    size_t      line_end  = hdr_block.find("\r\n");
    std::string firstLine = hdr_block.substr(0, line_end);

    // Request-Line
    std::cout << firstLine << std::endl;
    RequestLine rl = HttpParser::parseRequestLine(firstLine);
    client->setRequestLine(rl);

    // Headers
    std::string                                     rest = hdr_block.substr(line_end + 2);
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
    size_t             result = 0;
    std::istringstream iss(hex);
    iss >> std::hex >> result;
    if (result == SIZE_MAX)
        throw HttpException(400, "Bad Request");  // taille invalide
    if (result > MAX_CHUNK_SIZE)
        throw HttpException(413, "Payload Too Large");  // chunk trop gros
    return result;
}

bool SocketManager::parseClientBody(ClientSocket* client)
{
    std::string& buf  = client->getBuffer();
    BodyMode     mode = client->getBodyMode();

    if (mode == BODY_CONTENT_LENGTH)
    {
        size_t needed = client->getContentLength();
        if (needed > MAX_BODY_SIZE)
            throw HttpException(413, "Payload Too Large");
        if (buf.size() < needed)
            return false;
        // leave body in buffer until buildHttpRequest
        return true;
    }
    else if (mode == BODY_CHUNKED)
    {
        // chunked state machine, append to internal temporary storage
        while (true)
        {
            if (client->getChunkSize() == 0)
            {
                size_t pos = buf.find("\r\n");
                if (pos == std::string::npos)
                    return false;
                size_t chunkLen = hexToSize(buf.substr(0, pos));
                if (buf.size() + chunkLen > MAX_BODY_SIZE)
                    throw HttpException(413, "Payload Too Large");
                client->setChunkSize(chunkLen);
                buf.erase(0, pos + 2);
                if (chunkLen == 0)
                {
                    size_t end = buf.find("\r\n");
                    if (end == std::string::npos)
                        return false;
                    buf.erase(0, end + 2);
                    break;
                }
            }
            size_t needed = client->getChunkSize();
            if (buf.size() < needed + 2)
                return false;
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
    if (client->getBodyMode() == BODY_CONTENT_LENGTH)
    {
        req.body = client->getBuffer().substr(0, client->getContentLength());
        client->getBuffer().erase(0, client->getContentLength());
    }
    else if (client->getBodyMode() == BODY_CHUNKED)
    {
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

void SocketManager::cleanupRequest(ClientSocket* client) { client->resetParserState(); }

void SocketManager::closeConnection(int fd, int epoll_fd)
{
    // 1) Deregister from epoll if applicable
    if (epoll_fd >= 0)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        {
            // Log error but continue cleanup
            std::cerr << "epoll_ctl DEL failed for fd " << fd << ": " << strerror(errno)
                      << std::endl;
        }
    }

    // 2) Delete the ClientSocket object
    std::map<int, ClientSocket*>::iterator it = _clientSockets.find(fd);
    if (it != _clientSockets.end())
    {
        delete it->second;
        _clientSockets.erase(it);
    }

    // 3) Close the file descriptor
    if (close(fd) == -1)
    {
        std::cerr << "close() failed for fd " << fd << ": " << strerror(errno) << std::endl;
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
        if (req.headers.count("Connection") && req.headers["Connection"][0] == "close")
        {
            return false;  // plus de traitement sur cette socket
                           // closeConnection(fd, epoll_fd); REPLACER DASN EVENT LOOOP
        }
    }
    catch (const HttpException& he)
    {
        // Erreur de la requête (4xx / 5xx) :
        sendErrorResponse(fd, he.status(), he.what());
        // closeConnection(fd, epoll_fd);       // retire de epoll, delete ClientSocket, close(fd)
        return false;  // on arrête là pour ce fd
    }
    return true;  // on garde la connexion ouverte
}
