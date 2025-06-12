/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:56:33 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/10 17:31:12 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../SocketManager/SocketManager.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"
#include "WriteCallback.hpp"
#include "ErrorCallback.hpp"
#include "ReadCallback.hpp"
#include "CloseCallback.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>
#include <iostream>


WriteCallback::WriteCallback(int fd, SocketManager* manager, const std::string& header, int file_fd, int epoll_fd)
    : Callback(fd), _epoll_fd(epoll_fd), _manager(manager), _header(header), _headerSent(0), _headerDone(false),
      _file_fd(file_fd), _bodyOffset(0), _chunkBuffer(), _chunkSent(0), _fileDone(false), _finalChunkSent(false)
{
}

// Constructeur pour body en mémoire
WriteCallback::WriteCallback(int fd, SocketManager* manager, const std::string& header, const std::string& body, int epoll_fd)
    : Callback(fd), _epoll_fd(epoll_fd), _manager(manager), _header(header), _headerSent(0), _headerDone(false),
      _file_fd(-1), _body(body), _bodyOffset(0), _chunkBuffer(), _chunkSent(0), _fileDone(false), _finalChunkSent(false)
{
}
WriteCallback::WriteCallback(const WriteCallback& other)
    : Callback(other._fd),
      _epoll_fd(other._epoll_fd),
      _manager(other._manager),
      _header(other._header),
      _headerSent(other._headerSent),
      _headerDone(other._headerDone),
      _file_fd(other._file_fd),
      _body(other._body),
      _bodyOffset(other._bodyOffset),
      _chunkBuffer(other._chunkBuffer),
      _chunkSent(other._chunkSent),
      _fileDone(other._fileDone),
      _finalChunkSent(other._finalChunkSent)
{
}


WriteCallback::~WriteCallback()
{
    LOG_ERROR(DEBUG, CALLBACK_ERROR, "WriteCallback Destructor called.", __FUNCTION__);
}


void WriteCallback::execute() {
    ssize_t written;
    struct epoll_event ev;

    ev.events = EPOLLOUT;
    ev.data.fd = _fd;

    // 1. Envoi du header
    if (!_headerDone) {
        size_t toSend = _header.size() - _headerSent;
        written = send(_fd, _header.c_str() + _headerSent, toSend, MSG_NOSIGNAL);
        if (written < 0) { 
            _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _manager->getEpollFd(), FATAL));
            return;
        }
        _headerSent += written;
        if (_headerSent < _header.size()) {
            _manager->getCallbackQueue().push(new WriteCallback(*this));
            return;
        }
        _headerDone = true;
    }

    // 2. Envoi du chunk courant (si chunkBuffer à envoyer)
    if (!_chunkBuffer.empty()) {
        size_t toSend = _chunkBuffer.size() - _chunkSent;
        written = send(_fd, _chunkBuffer.c_str() + _chunkSent, toSend, MSG_NOSIGNAL);
        if (written < 0) {
            
            _manager->getCallbackQueue().push(new ErrorCallback(_fd, _manager, _manager->getEpollFd(), FATAL));
            return;
        }
        _chunkSent += written;
        if (_chunkSent < _chunkBuffer.size()) {
            _manager->getCallbackQueue().push(new WriteCallback(*this));
            return;
        }
        _chunkBuffer.clear();
        _chunkSent = 0;

        if (_finalChunkSent) {
            finalizeConnection();
            return;
        }
        // NE PAS RETURN : continuer pour générer un nouveau chunk si besoin
    }

    // 3. Générer un chunk à partir du body ou du fichier
    if (!_fileDone) {
        char buf[8192];
        ssize_t nread = -1;
        if (_file_fd != -1) {
            nread = read(_file_fd, buf, sizeof(buf));
        } else if (_bodyOffset < _body.size()) {
            nread = std::min<size_t>(sizeof(buf), _body.size() - _bodyOffset);
            memcpy(buf, _body.c_str() + _bodyOffset, nread);
            _bodyOffset += nread;
        } else if (_file_fd == -1 && _bodyOffset >= _body.size() && !_fileDone) {
            nread = 0;
        }

        if (nread < 0) {
            // read() a échoué, on abandonne la connexion// GESTION ERROR
            return;
        }
        if (nread == 0) {
            _fileDone = true;
            if (!_finalChunkSent) {
                // On prépare le chunk final (0\r\n\r\n)
                _chunkBuffer = "0\r\n\r\n";
                _chunkSent = 0;
                _finalChunkSent = true;
                _manager->getCallbackQueue().push(new WriteCallback(*this));
                return;
            }
            return;
        }

        // Génère un chunk : taille (hex) + \r\n + data + \r\n
        std::ostringstream chunkoss;
        chunkoss << std::hex << nread << "\r\n";
        _chunkBuffer = chunkoss.str();
        _chunkBuffer.append(buf, nread);
        _chunkBuffer.append("\r\n");
        _chunkSent = 0;
        _manager->getCallbackQueue().push(new WriteCallback(*this));
        return;
    }
}

// Nouvelle méthode privée : finalisation de la connexion après le dernier chunk envoyé
void WriteCallback::finalizeConnection() {
    std::cout << "[WriteCallback] ALL DONE: switching fd to EPOLLIN" << std::endl;
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET ;
    ev.data.fd = _fd;
    _manager->safeEpollCtlClient(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev);
    if (_file_fd != -1) {
        std::cout << "[WriteCallback] FILE: closing file_fd=" << _file_fd << std::endl;
        close(_file_fd);
    }
}

int WriteCallback::get_file_fd() const
{ return (_file_fd); }
