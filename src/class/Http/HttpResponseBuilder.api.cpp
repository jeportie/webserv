/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.api.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 08:13:55 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/12 08:31:06 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponseBuilder.hpp"
#include "HttpCGI.hpp"
#include "HttpException.hpp"
#include "ContentGenerator.hpp"
#include "RequestValidator.hpp"
#include "ResponseFormatter.hpp"
#include "StatusUtils.hpp"
#include <cstring>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <string>



std::string HttpResponseBuilder::resolveTargetPath()
{
    std::string              root;
    std::vector<std::string> indexFiles;
    bool                     indexIsSet = false;
    std::string              defaultFile;
    std::string              uri = _request.path;
    std::string              routepath;

    if (_validator.hasMatchedRoute())
    {
        const RouteConfig&  route      = _validator.getMatchedRoute();
        const ServerConfig& serverConf = _validator.getServerConfig();
        routepath                      = route.path;
        if (route.root.empty())
        {
            root = serverConf.root;
        }
        else
            root = route.root;
        indexFiles  = route.indexFiles;
        indexIsSet  = route.indexIsSet;
        defaultFile = route.defaultFile;
    }
    else
    {
        const ServerConfig& serverConf = _validator.getServerConfig();
        root                           = serverConf.root;
        indexFiles                     = serverConf.indexFiles;
        indexIsSet                     = serverConf.indexIsSet;
        defaultFile                    = "index.html";  // Fallback
    }


    // Retirer la partie routepath de l'uri
    if (!routepath.empty() && routepath != "/" && uri.find(routepath) == 0)
    {
        uri = uri.substr(routepath.length());
        if (!uri.empty() && uri[0] == '/')
            uri = uri.substr(1);
    }

    // Construction du chemin complet
    if (!root.empty() && root[root.size() - 1] != '/' && uri[0] != '/')
        root += "/";
    std::string fullPath = root + uri;

    if (isDirectory(fullPath))
    {
        if (fullPath[fullPath.size() - 1] != '/')
            fullPath += "/";

        // Si des fichiers index ont été définis explicitement
        if (indexIsSet)
        {
            for (std::vector<std::string>::iterator it = indexFiles.begin(); it != indexFiles.end();
                 ++it)
            {
                std::string candidate = fullPath + *it;
                if (fileExists(candidate))
                    return candidate;
            }
            // Aucun fichier trouvé → on retourne le dossier brut (autoindex ?)
            return fullPath;
        }

        // Sinon on peut fallback sur defaultFile (même si index non set)
        std::string fallback = fullPath + defaultFile;
        if (!defaultFile.empty() && fileExists(fallback))
            return fallback;

        // Aucun fichier d’index trouvé, retour du dossier
        return fullPath;
    }

    return fullPath;
}

void HttpResponseBuilder::setConnection()
{
    std::map<std::string, std::vector<std::string> >::const_iterator it
        = _request.headers.find("Connection");

    if (it == _request.headers.end())
    {
        // Header "Connection" absent dans la requête
        if (_request.http_major == 1 && _request.http_minor == 0)
        {
            // HTTP/1.0 par défaut : connexion close
            _response.setHeader("Connection", "close");
        }
        else
        {
            // HTTP/1.1 et plus : connexion keep-alive par défaut
            _response.setHeader("Connection", "keep-alive");
        }
    }
    else
    {
        // Header "Connection" présent, vérifie la valeur
        std::string connection = "";

        if (!it->second.empty())
            connection = it->second[0];


        if (connection == "keep-alive")
            _response.setHeader("Connection", "keep-alive");
        else
            _response.setHeader("Connection", "close");
    }
}


// ----------- Fonctions utilitaires anonymes -----------

std::string extractBoundary(const std::string& contentType)
{
    const std::string multipartPrefix = "multipart/form-data; boundary=";
    if (contentType.compare(0, multipartPrefix.size(), multipartPrefix) == 0)
        return "--" + contentType.substr(multipartPrefix.size());
    return std::string();
}


bool isValidUploadDir(const std::string& dir)
{
    struct stat sb;
    return stat(dir.c_str(), &sb) == 0 &&
           S_ISDIR(sb.st_mode) &&
           access(dir.c_str(), W_OK | X_OK) == 0;
}

bool isValidFilename(const std::string& filename)
{
    return filename.find("../") == std::string::npos && filename.find('/') == std::string::npos;
}

std::string makeUniqueFilepath(const std::string& dir, const std::string& filename)
{
    std::string filepath = dir + "/" + filename;
    struct stat sb;
    if (stat(filepath.c_str(), &sb) == 0)
    {
        std::ostringstream oss;
        oss << dir << "/" << time(NULL) << "_" << filename;
        filepath = oss.str();
    }
    return filepath;
}


bool saveToFile(const std::string& filepath, const std::string& data)
{
    std::ofstream ofs(filepath.c_str(), std::ios::binary);
    if (!ofs.is_open())
        return false;
    ofs.write(data.data(), data.size());
    if (!ofs)
    {
        ofs.close();
        return false;
    }
    ofs.close();
    if (!ofs)
        return false;
    return true;
}



bool HttpResponseBuilder::storeUploadedFile(HttpRequest&       request,
                                            const std::string& uploadStorePath)
{
    // 1. Vérifier Content-Type
    if (!request.headers.count("Content-Type"))
        throw HttpException(400,
                            "Bad Request : Need a Content-Type when POST /uploads",
                            _validator.getErrorPage(400));

    std::string contentType = request.headers["Content-Type"][0];
    std::string boundary    = extractBoundary(contentType);
    if (boundary.empty())
        throw HttpException(400, "Bad Request : No boundary found", _validator.getErrorPage(400));

    // 2. Vérifier dossier upload
    if (!isValidUploadDir(uploadStorePath))
        throw HttpException(
            500, "Internal Server Error : Invalid Upload Directory", _validator.getErrorPage(500));

    // 3. Découper le body en parties
    const std::string& body = request.body;
    size_t             pos  = 0;
    size_t               end;

    pos = body.find(boundary, pos);
    while (pos != std::string::npos)
    {
        pos += boundary.size();
        // fin de message ?
        if (body.compare(pos, 2, "--") == 0)
            break;
        
        if (body.compare(pos, 2, "\r\n") == 0)
            pos += 2;
        
        end = body.find("\r\n\r\n", pos);
        if (end == std::string::npos)
            break;
        
        std::string partHeaders = body.substr(pos, end - pos);
        pos                     = end + 4;
        size_t dispPos          = partHeaders.find("Content-Disposition:");
        size_t filenamePos      = partHeaders.find("filename=\"");
        
        if (dispPos == std::string::npos || filenamePos == std::string::npos)
        {
            pos = body.find(boundary, pos);
            continue;
        }

        filenamePos += 10;
        size_t filenameEnd = partHeaders.find('"', filenamePos);
        if (filenameEnd == std::string::npos)
        {
            pos = body.find(boundary, pos);
            continue;
        }
    
        std::string filename = partHeaders.substr(filenamePos, filenameEnd - filenamePos);
        if (!isValidFilename(filename))
        {
            pos = body.find(boundary, pos);
            continue;
        }
    
        std::string filepath = makeUniqueFilepath(uploadStorePath, filename);
        end                  = body.find(boundary, pos);
        if (end == std::string::npos)
            break;
    
        size_t fileDataEnd = end;
        if (fileDataEnd >= 2 && body[fileDataEnd - 2] == '\r' && body[fileDataEnd - 1] == '\n')
            fileDataEnd -= 2;
    
        std::string fileData = body.substr(pos, fileDataEnd - pos);
        if (saveToFile(filepath, fileData))
            return true;  // on enregistre seulement le premier fichier
        
        pos = body.find(boundary, end);  // passer au suivant seulement si on ne retourne pas
    }
    return false;
}
