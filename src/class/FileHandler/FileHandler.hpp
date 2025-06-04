/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 14:03:23 by jeportie          #+#    #+#             */
/*   Updated: 2025/06/04 14:03:46 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <map>
#include "../Http/HttpResponse.hpp"

class FileHandler
{
private:
    std::string                        _rootDir;
    std::map<std::string, std::string> _mimeTypes;

public:
    FileHandler(const std::string& rootDir);
    ~FileHandler();

    HttpResponse handleGET(const std::string& uri);
    HttpResponse handlePOST(const std::string& uri, const std::string& body);
    HttpResponse handleDELETE(const std::string& uri);

private:
    std::string getMimeType(const std::string& filename);
    bool        fileExists(const std::string& path);
    std::string readFile(const std::string& path);
    bool        writeFile(const std::string& path, const std::string& content);
    bool        deleteFile(const std::string& path);
    std::string mapUriToPath(const std::string& uri);
    bool        isPathSafe(const std::string& path);
};

#endif  // ************************************************** FILEHANDLER_HPP //
