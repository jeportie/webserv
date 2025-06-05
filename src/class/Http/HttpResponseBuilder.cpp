/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:00:11 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/05 14:59:14 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponseBuilder.hpp"
#include "HttpCGI.hpp"
#include "HttpException.hpp"
#include "ContentGenerator.hpp"
#include "RequestValidator.hpp"
#include "ResponseFormatter.hpp"
#include "StatusUtils.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>

// Constructeur
HttpResponseBuilder::HttpResponseBuilder(const HttpRequest& req, const RequestValidator& validator)
: _request(req)
, _validator(validator)
, _response()
{
}

std::string toString(size_t value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

// Point d’entrée principal
void HttpResponseBuilder::buildResponse()
{
    switch (_request.method)
    {
        case HttpRequest::METHOD_GET:
            handleGET();
            break;
        case HttpRequest::METHOD_POST:
            handlePOST();
            break;
        case HttpRequest::METHOD_DELETE:
            handleDELETE();
            break;
        default:
            throw HttpException(405, "Method Not Allowed", _validator.getErrorPage(405));
            break;
    }
}

// Accès à la réponse finale
const HttpResponse& HttpResponseBuilder::getResponse() const { return _response; }

// Gestion GET
void HttpResponseBuilder::handleGET()
{
	std::string scriptPath = resolveTargetPath();
    // rajout du bloc d'executation du CGi si presence d'un CGI, le CGI prime sur le reste
	if (_validator.hasMatchedRoute())
	{
		const RouteConfig &route = _validator.getMatchedRoute();
		if (!route.cgiExecutor.first.empty())
		{
			// Si la requête cible le script, on exécute le CGI
			if (!isExecutable(route.cgiExecutor.second) || !isExecutable(scriptPath))
				throw HttpException(403, "Forbidden",
					_validator.getErrorPage(403));
			std::string output = runCgiScript(_request,
					route.cgiExecutor.second, _validator);
			_response.setStatus(200, "OK");
            _response.parseCgiOutputAndSet(output);
			setConnection();
			return ;
		}
	}
	std::string path = resolveTargetPath();
	// Si le chemin est un répertoire sans index
	if (isDirectory(path))
	{
		if (!_validator.hasMatchedRoute())
		{
			// Pas de route correspondante, refuse l'accès ou 404
			throw HttpException(404, "Not Found", _validator.getErrorPage(404));
		}
		const RouteConfig &route = _validator.getMatchedRoute();
		if (route.autoindex)
		{
			std::string html = generateAutoIndexPage(path, _request.path);
			_response.setStatus(200, "OK");
			_response.setHeader("Content-Type", "text/html");
			_response.setHeader("Content-Length", toString(html.size()));
			_response.setBody(html);
		}
		else
		{
			throw HttpException(403, "Forbidden", _validator.getErrorPage(403));
		}
		return ;
	}
	// Sinon, comportement standard
	if (!fileExists(path))
		throw HttpException(404, "Not Found", _validator.getErrorPage(404));
	std::string content;
	if (!readFileContent(path, content))
		throw HttpException(500, "Internal Server Error",
			_validator.getErrorPage(500));
	std::string mimeType = getMimeType(path);
	_response.setStatus(200, "OK");
	_response.setHeader("Content-Type", mimeType);
	_response.setHeader("Content-Length", toString(content.size()));
	_response.setBody(content);
	setConnection();
}

// Gestion POST (exemple simple : echo ou cgi)
void HttpResponseBuilder::handlePOST()
{
	std::string scriptPath = resolveTargetPath();
	bool	success;

	if (_validator.hasMatchedRoute())
	{
		const RouteConfig &route = _validator.getMatchedRoute();
		if (!route.cgiExecutor.first.empty())
		{
			if (!isExecutable(route.cgiExecutor.second) || !isExecutable(scriptPath))

				throw HttpException(403, "Forbidden",
					_validator.getErrorPage(403));
			std::string output = runCgiScript(_request,
					route.cgiExecutor.second, _validator);
			_response.setStatus(200, "OK");
			_response.parseCgiOutputAndSet(output);
		}
		else if (route.uploadEnabled && !route.uploadStore.empty())
		{
			// Upload activé sur la route
			success = storeUploadedFile(_request, route.uploadStore);
			if (success)
			{
				_response.setStatus(201, "Created");
				_response.setBody("File uploaded successfully.");
			}
			else
			{
				throw HttpException(500, "Internal Server Error",
					_validator.getErrorPage(500));
			}
		}
		else
		{
			// Pas de CGI : renvoyer le corps brut
			_response.setStatus(200, "OK");
			_response.setHeader("Content-Type", "text/plain");
			_response.setBody(_request.body);
		}
	}
	else
	{
		// Aucun bloc de route ne matche -> POST non accepté ici
		throw HttpException(404, "Not Found", _validator.getErrorPage(404));
	}
	setConnection();
}

// fonction pour supprimer le fichier
bool deleteFile(const std::string& path)
{
    // std::remove retourne 0 si succès
    return (unlink(path.c_str()) == 0);
}

// Gestion DELETE (simplifiée)
void HttpResponseBuilder::handleDELETE()
{
	std::string path = resolveTargetPath();
	if (!fileExists(path))
	{
		throw HttpException(404, "Not Found", _validator.getErrorPage(404));
	}
	if (!deleteFile(path))
	{
		throw HttpException(500, "Internal Server Error",
			_validator.getErrorPage(500));
	}
	// Réponse 204 No Content est classique pour un DELETE réussi sans corps de réponse
	_response.setStatus(204, "No Content");
	// Si tu veux renvoyer 200 OK avec un message, décommente ces lignes:
	// _response.setStatus(200, "OK");
	// _response.setHeader("Content-Type", "text/plain");
	// _response.setBody("File deleted successfully.");
	setConnection();
}

std::string HttpResponseBuilder::resolveTargetPath()
{
    std::string              root;
    std::vector<std::string> indexFiles;
    bool                     indexIsSet = false;
    std::string              defaultFile;
    std::string              uri = _request.path;

    if (_validator.hasMatchedRoute())
    {
        const RouteConfig& route = _validator.getMatchedRoute();
		const ServerConfig& serverConf = _validator.getServerConfig();
		if (route.root.empty())
		{
			root = serverConf.root;
		}
		else
       	 root                     = route.root;
        indexFiles               = route.indexFiles;
        indexIsSet               = route.indexIsSet;
        defaultFile              = route.defaultFile;
    }
    else
    {
        const ServerConfig& serverConf = _validator.getServerConfig();
        root                           = serverConf.root;
        indexFiles                     = serverConf.indexFiles;
        indexIsSet                     = serverConf.indexIsSet;
        defaultFile                    = "index.html";  // Fallback
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

std::string extractBoundary(const std::string& contentType) {
    const std::string multipartPrefix = "multipart/form-data; boundary=";
    if (contentType.compare(0, multipartPrefix.size(), multipartPrefix) == 0)
        return "--" + contentType.substr(multipartPrefix.size());
    return std::string();
}

bool isValidUploadDir(const std::string& dir) {
    struct stat sb;
    return stat(dir.c_str(), &sb) == 0 && (sb.st_mode & S_IFDIR);
}

bool isValidFilename(const std::string& filename) {
    return filename.find("../") == std::string::npos && filename.find('/') == std::string::npos;
}

std::string makeUniqueFilepath(const std::string& dir, const std::string& filename) {
    std::string filepath = dir + "/" + filename;
    struct stat sb;
    if (stat(filepath.c_str(), &sb) == 0) {
        std::ostringstream oss;
        oss << dir << "/" << time(NULL) << "_" << filename;
        filepath = oss.str();
    }
    return filepath;
}

bool saveToFile(const std::string& filepath, const std::string& data) {
    std::ofstream ofs(filepath.c_str(), std::ios::binary);
    if (!ofs)
        return false;
    ofs.write(data.data(), data.size());
    ofs.close();
    return true;
}


// ----------- Fonction principale découpée -------------

bool HttpResponseBuilder::storeUploadedFile(HttpRequest& request, const std::string& uploadStorePath)
{
    // 1. Vérifier Content-Type
    if (!request.headers.count("Content-Type"))
        return false;
		
    std::string contentType = request.headers["Content-Type"][0];
    std::string boundary = extractBoundary(contentType);
    if (boundary.empty())
        return false;

    // 2. Vérifier dossier upload
    if (!isValidUploadDir(uploadStorePath))
        return false;

    // 3. Découper le body en parties
    const std::string& body = request.body;
    size_t pos = 0, end;
	
    while ((pos = body.find(boundary, pos)) != std::string::npos) {
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
        pos = end + 4;
        size_t dispPos = partHeaders.find("Content-Disposition:");
        size_t filenamePos = partHeaders.find("filename=\"");
		
        if (dispPos == std::string::npos || filenamePos == std::string::npos)
            continue;

        filenamePos += 10;
        size_t filenameEnd = partHeaders.find('"', filenamePos);
        if (filenameEnd == std::string::npos)
            continue;
			
        std::string filename = partHeaders.substr(filenamePos, filenameEnd - filenamePos);
        if (!isValidFilename(filename))
            continue;

        std::string filepath = makeUniqueFilepath(uploadStorePath, filename);
        end = body.find(boundary, pos);
        if (end == std::string::npos)
            break;
			
        size_t fileDataEnd = end;
        if (fileDataEnd >= 2 && body[fileDataEnd - 2] == '\r' && body[fileDataEnd - 1] == '\n')
            fileDataEnd -= 2;
			
        std::string fileData = body.substr(pos, fileDataEnd - pos);
        if (saveToFile(filepath, fileData))
            return true; // on enregistre seulement le premier fichier
        else
            continue;
    }
    return false;
}
