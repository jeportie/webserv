/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestData.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:31:21 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 16:57:41 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ClientSocket.hpp"
#include "../../../include/webserv.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

// Constructor - Destructor

RequestData::RequestData()
: _buffer()
, _isHeadersParsed(false)
, _bodyMode(BODY_NONE)
, _contentLength(0)
, _requestLine()
, _parsedHeaders()
, _isChunked(false)
, _chunkSize(0)
, _bodyAccumulator()
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_REQUESTDATA_CONSTRUCTOR, __FUNCTION__);
}

RequestData::~RequestData(void)
{
    LOG_ERROR(
        DEBUG, SOCKET_ERROR, LOG_REQUESTDATA_DESTRUCTOR, __FUNCTION__);
}

// GETTERS

std::string& RequestData::getBuffer() { return (_buffer); }

size_t RequestData::getContentLength() const { return (_contentLength); }

SVSMAP RequestData::getParsedHeaders() const { return (_parsedHeaders); }

bool RequestData::getIsHeadersParsed() const { return (_isHeadersParsed); }

RequestLine RequestData::getRequestLine() const { return (_requestLine); }

BodyMode RequestData::getBodyMode() const { return (_bodyMode); }

std::string& RequestData::getBodyAccumulator() { return _bodyAccumulator; }

bool RequestData::getIsChunked() const { return (_isChunked); }

size_t RequestData::getChunkSize() const { return (_chunkSize); }

// SETTERS

void RequestData::setHeadersParsed(bool parsed) { _isHeadersParsed = parsed; }

void RequestData::setContentLength(size_t length) { _contentLength = length; }

void RequestData::setRequestLine(RequestLine rl) { _requestLine = rl; }

void RequestData::setBodyMode(BodyMode mode) { _bodyMode = mode; }

void RequestData::setChunked(bool c) { _isChunked = c; }

void RequestData::setChunkSize(size_t s) { _chunkSize = s; }

void RequestData::setParsedHeaders(SVSMAP hdrs)
{
    _parsedHeaders = hdrs;
}
