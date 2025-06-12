/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:00:11 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/12 08:14:41 by fsalomon         ###   ########.fr       */
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


std::string getHttpErrorMessage(int code)
{
    switch (code)
    {
        case 400:
            return "Bad Request";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        // ... Ajoute ce que tu veux
        default:
            return "Error";
    }
}

void HttpResponseBuilder::setChunkedHeaders()
{
    _response.setHeader("Transfer-Encoding", "chunked");
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

void HttpResponseBuilder::executeReturnDirective(const RouteConfig& route)
{
    // Ici, on prend le premier (ou celui correspondant au code voulu)
    int                code = route.returnCodes.begin()->first;
    const std::string& url  = route.returnCodes.begin()->second;
    if (!url.empty())
    {
        // C'est une redirection (301, 302, etc.)
        _response.setStatus(code, "Moved");  // Tu peux ajuster le message selon code
        _response.setHeader("Location", url);
        setChunkedHeaders();
        _response.setBody("<html><body><h1>" + toString(code) + " Redirect</h1><p><a href=\"" + url
                          + "\">Redirecting</a></p></body></html>");
        setConnection();
        return;
    }
    else
    {
        std::string errorMessage = getHttpErrorMessage(code);
        throw HttpException(code, errorMessage, _validator.getErrorPage(code));
        setConnection();
        return;
    }
}

// Gestion GET
void HttpResponseBuilder::handleGET()
{
    std::string scriptPath = resolveTargetPath();
    // rajout du bloc d'executation du CGi si presence d'un CGI, le CGI prime sur le reste
    if (_validator.hasMatchedRoute())
    {
        const RouteConfig& route = _validator.getMatchedRoute();
        if (!route.returnCodes.empty())
        {
            executeReturnDirective(route);
            return;
        }

        if (!route.cgiExecutor.first.empty())
        {

        if (!fileExists(route.cgiExecutor.second) || !fileExists(scriptPath))
            throw HttpException(404, "Not Found", _validator.getErrorPage(404));

        if (!isExecutable(route.cgiExecutor.second) || !isExecutable(scriptPath))
            throw HttpException(403, "Forbidden", _validator.getErrorPage(403));

        std::string output = runCgiScript(_request, route.cgiExecutor.second, _validator);
        _response.setStatus(200, "OK");
        setChunkedHeaders();
        _response.parseCgiOutputAndSet(output);
        setConnection();
        return;
        
        }
    }
    std::string path = resolveTargetPath();
    std::cout << "PATH =" << path << std::endl;
    // Si le chemin est un répertoire sans index
    if (isDirectory(path))
    {
        if (!_validator.hasMatchedRoute())
        {
            // Pas de route correspondante, refuse l'accès ou 404
            throw HttpException(404, "Not Found", _validator.getErrorPage(404));
        }
        const RouteConfig& route = _validator.getMatchedRoute();
        if (route.autoindex)
        {
            std::string html = generateAutoIndexPage(path, _request.path);
            _response.setStatus(200, "OK");
            _response.setHeader("Content-Type", "text/html");
            setChunkedHeaders();
            _response.setBody(html);
        }
        else
        {
            throw HttpException(403, "Forbidden", _validator.getErrorPage(403));
        }
        return;
    }
    // Sinon, comportement standard
    if (!fileExists(path))
    {
        throw HttpException(404, "Not Found", _validator.getErrorPage(404));
    }
    // std::string content;
    // if (!readFileContent(path, content))
    // 	throw HttpException(500, "Internal Server Error",
    // 		_validator.getErrorPage(500));
    std::string mimeType = getMimeType(path);
    _response.setStatus(200, "OK");
    _response.setHeader("Content-Type", mimeType);
    setChunkedHeaders();
    _response.setFileToStream(path);
    setConnection();
}

// Gestion POST (exemple simple : echo ou cgi)
void HttpResponseBuilder::handlePOST()
{
    std::string scriptPath = resolveTargetPath();
    bool        success;

    if (_validator.hasMatchedRoute())
    {
        const RouteConfig& route = _validator.getMatchedRoute();
        if (!route.returnCodes.empty())
        {
            executeReturnDirective(route);
            return;
        }
        if (!route.cgiExecutor.first.empty())
        {
            if (!isExecutable(route.cgiExecutor.second) || !isExecutable(scriptPath))

                throw HttpException(403, "Forbidden", _validator.getErrorPage(403));
            std::string output = runCgiScript(_request, route.cgiExecutor.second, _validator);
            _response.setStatus(200, "OK");
            _response.parseCgiOutputAndSet(output);
        }
        else if (route.uploadEnabled && !route.uploadStore.empty())
        {
            std::string fullpath;
            // Upload activé sur la route
            if (route.root.empty())
                fullpath = resolvePath(route.uploadStore, _validator.getServerConfig().root);
            else
                fullpath = resolvePath(route.uploadStore, route.root);
            success = storeUploadedFile(_request, fullpath);
            if (success)
            {
                _response.setStatus(201, "Created");
                setChunkedHeaders();
                _response.setBody("File uploaded successfully.");
            }
            else
            {
                throw HttpException(500,
                                    "Internal Server Error : File upload failed",
                                    _validator.getErrorPage(500));
            }
        }
        else
        {
            // Pas de CGI : renvoyer le corps brut
            _response.setStatus(200, "OK");
            setChunkedHeaders();
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

bool deleteFile(const std::string& path)
{
    //retourne 0 si succès
    return (unlink(path.c_str()) == 0);
}

void HttpResponseBuilder::handleDELETE()
{
    if (_validator.hasMatchedRoute())
    {
        const RouteConfig& route = _validator.getMatchedRoute();
        if (!route.returnCodes.empty())
        {
            executeReturnDirective(route);
            return;
        }
    }
    std::string path = resolveTargetPath();
    std::cout << "PATH =" << path << std::endl;
    if (!fileExists(path))
    {
        throw HttpException(404, "Not Found", _validator.getErrorPage(404));
    }
    if (!deleteFile(path))
    {
        throw HttpException(500, "Internal Server Error", _validator.getErrorPage(500));
    }
    _response.setStatus(200, "OK");
    _response.setHeader("Content-Type", "text/plain");
    setChunkedHeaders();
    _response.setBody("File deleted successfully.");
    setConnection();
}
