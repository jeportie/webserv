/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WriteCallback.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 12:55:20 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/09 17:10:57 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WRITECALLBACK_HPP
#define WRITECALLBACK_HPP

#include "Callback.hpp"
#include <string>

class SocketManager;

// Write data to client callback
class WriteCallback : public Callback
{
public:

    WriteCallback(int fd, SocketManager* manager, const std::string& header, int file_fd, int epoll_fd);
    WriteCallback(int fd, SocketManager* manager, const std::string& header, const std::string& body, int epoll_fd);
    WriteCallback(const WriteCallback& other);
    virtual ~WriteCallback();

    virtual void execute();
    void finalizeConnection();

private:

    int _epoll_fd;
    SocketManager* _manager;
    std::string _header;
    size_t _headerSent;
    bool _headerDone;

    int _file_fd; // -1 si pas de fichier à streamer
    std::string _body;
    size_t _bodyOffset;

    std::string _chunkBuffer; // chunk hex + data + \r\n
    size_t _chunkSent;
    bool _fileDone, _finalChunkSent;
};

#endif  // ************************************************ WRITECALLBACK_HPP //
