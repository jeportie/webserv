/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 12:00:00 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/04 12:00:00 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileHandler.hpp"
#include "../Errors/ErrorHandler.hpp"
#include "../../../include/webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

// Constructor: Initialize with root directory and MIME types
FileHandler::FileHandler(const std::string& rootDir)
: _rootDir(rootDir)
{
    LOG_ERROR(DEBUG, FILE_HANDLER_ERROR, "FileHandler Constructor called.", __FUNCTION__);
    
    // Initialize MIME type mapping
    _mimeTypes[".html"] = "text/html";
    _mimeTypes[".htm"] = "text/html";
    _mimeTypes[".css"] = "text/css";
    _mimeTypes[".js"] = "application/javascript";
    _mimeTypes[".json"] = "application/json";
    _mimeTypes[".png"] = "image/png";
    _mimeTypes[".jpg"] = "image/jpeg";
    _mimeTypes[".jpeg"] = "image/jpeg";
    _mimeTypes[".gif"] = "image/gif";
    _mimeTypes[".svg"] = "image/svg+xml";
    _mimeTypes[".ico"] = "image/x-icon";
    _mimeTypes[".txt"] = "text/plain";
    _mimeTypes[".pdf"] = "application/pdf";
    _mimeTypes[".zip"] = "application/zip";
}

// Destructor
FileHandler::~FileHandler()
{
    LOG_ERROR(DEBUG, FILE_HANDLER_ERROR, "FileHandler Destructor called.", __FUNCTION__);
}

// Handle GET request for a static file
HttpResponse FileHandler::handleGET(const std::string& uri)
{
    HttpResponse response;
    std::string path = mapUriToPath(uri);
    
    // Check if path is safe
    if (!isPathSafe(path))
    {
        response.setStatus(403, "Forbidden");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Forbidden: Access denied");
        return response;
    }
    
    // Check if file exists
    if (!fileExists(path))
    {
        response.setStatus(404, "Not Found");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("File not found");
        return response;
    }
    
    // Read file content
    std::string content = readFile(path);
    if (content.empty() && errno != 0)
    {
        response.setStatus(500, "Internal Server Error");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Failed to read file");
        return response;
    }
    
    // Set response headers and body
    response.setStatus(200, "OK");
    response.setHeader("Content-Type", getMimeType(path));
    response.setHeader("Content-Length", toString(content.size()));
    response.setBody(content);
    
    return response;
}

// Handle POST request to write/update a file
HttpResponse FileHandler::handlePOST(const std::string& uri, const std::string& body)
{
    HttpResponse response;
    std::string path = mapUriToPath(uri);
    
    // Check if path is safe
    if (!isPathSafe(path))
    {
        response.setStatus(403, "Forbidden");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Forbidden: Access denied");
        return response;
    }
    
    // Write content to file
    bool success = writeFile(path, body);
    if (!success)
    {
        response.setStatus(500, "Internal Server Error");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Failed to write file");
        return response;
    }
    
    // Set response for successful write
    response.setStatus(201, "Created");
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Content-Length", "22");
    response.setBody("File created/updated");
    
    return response;
}

// Handle DELETE request to remove a file
HttpResponse FileHandler::handleDELETE(const std::string& uri)
{
    HttpResponse response;
    std::string path = mapUriToPath(uri);
    
    // Check if path is safe
    if (!isPathSafe(path))
    {
        response.setStatus(403, "Forbidden");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Forbidden: Access denied");
        return response;
    }
    
    // Check if file exists
    if (!fileExists(path))
    {
        response.setStatus(404, "Not Found");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("File not found");
        return response;
    }
    
    // Delete the file
    bool success = deleteFile(path);
    if (!success)
    {
        response.setStatus(500, "Internal Server Error");
        response.setHeader("Content-Type", "text/plain");
        response.setBody("Failed to delete file");
        return response;
    }
    
    // Set response for successful deletion
    response.setStatus(204, "No Content");
    
    return response;
}

// Get MIME type based on file extension
std::string FileHandler::getMimeType(const std::string& filename)
{
    std::string::size_type dot = filename.rfind('.');
    if (dot != std::string::npos)
    {
        std::string ext = filename.substr(dot);
        std::map<std::string, std::string>::iterator it = _mimeTypes.find(ext);
        if (it != _mimeTypes.end())
            return it->second;
    }
    
    // Default MIME type for unknown extensions
    return "application/octet-stream";
}

// Check if file exists
bool FileHandler::fileExists(const std::string& path)
{
    struct stat fileStat;
    return (stat(path.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode));
}

// Read file content
std::string FileHandler::readFile(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        LOG_SYSTEM_ERROR(WARNING, FILE_HANDLER_ERROR, "Failed to open file for reading", __FUNCTION__);
        return "";
    }
    
    std::ostringstream ss;
    ss << file.rdbuf();
    file.close();
    
    return ss.str();
}

// Write content to file
bool FileHandler::writeFile(const std::string& path, const std::string& content)
{
    // Create directories if they don't exist
    std::string::size_type lastSlash = path.rfind('/');
    if (lastSlash != std::string::npos)
    {
        std::string dir = path.substr(0, lastSlash);
        std::string command = "mkdir -p " + dir;
        int result = system(command.c_str());
        if (result != 0)
        {
            LOG_SYSTEM_ERROR(WARNING, FILE_HANDLER_ERROR, "Failed to create directory", __FUNCTION__);
            return false;
        }
    }
    
    std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        LOG_SYSTEM_ERROR(WARNING, FILE_HANDLER_ERROR, "Failed to open file for writing", __FUNCTION__);
        return false;
    }
    
    file.write(content.c_str(), content.size());
    file.close();
    
    return !file.fail();
}

// Delete a file
bool FileHandler::deleteFile(const std::string& path)
{
    return (unlink(path.c_str()) == 0);
}

// Map URI to filesystem path
std::string FileHandler::mapUriToPath(const std::string& uri)
{
    // Remove query parameters if present
    std::string cleanUri = uri;
    std::string::size_type queryPos = uri.find('?');
    if (queryPos != std::string::npos)
        cleanUri = uri.substr(0, queryPos);
    
    // Ensure root directory ends with a slash
    std::string root = _rootDir;
    if (!root.empty() && root[root.size() - 1] != '/')
        root += "/";
    
    // Remove leading slash from URI if present
    if (!cleanUri.empty() && cleanUri[0] == '/')
        cleanUri = cleanUri.substr(1);
    
    return root + cleanUri;
}

// Check if path is safe (prevent directory traversal)
bool FileHandler::isPathSafe(const std::string& path)
{
    // Normalize path (resolve ../, ./, etc.)
    char resolvedPath[PATH_MAX];
    if (realpath(path.c_str(), resolvedPath) == NULL)
    {
        // If realpath fails, check if it's because the file doesn't exist
        // or because of a permission error
        if (errno == ENOENT)
        {
            // File doesn't exist, check if parent directory is safe
            std::string::size_type lastSlash = path.rfind('/');
            if (lastSlash != std::string::npos)
            {
                std::string parentDir = path.substr(0, lastSlash);
                return isPathSafe(parentDir);
            }
            return true; // No parent directory, assume safe
        }
        
        LOG_SYSTEM_ERROR(WARNING, FILE_HANDLER_ERROR, "Failed to resolve path", __FUNCTION__);
        return false;
    }
    
    // Check if resolved path starts with root directory
    char resolvedRoot[PATH_MAX];
    if (realpath(_rootDir.c_str(), resolvedRoot) == NULL)
    {
        LOG_SYSTEM_ERROR(ERROR, FILE_HANDLER_ERROR, "Failed to resolve root directory", __FUNCTION__);
        return false;
    }
    
    return (strncmp(resolvedPath, resolvedRoot, strlen(resolvedRoot)) == 0);
}

// Helper function to convert size_t to string
std::string toString(size_t value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}