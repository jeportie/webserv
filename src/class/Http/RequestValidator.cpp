/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 15:13:38 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:08:23 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestValidator.hpp"
#include "HttpException.hpp"
#include <algorithm>

RequestValidator::RequestValidator(const HttpRequest& req, const ServerConfig& serverConfig)
: _req(req)
, _serverConfig(serverConfig)
{
}

RequestValidator::~RequestValidator() {}

static bool isNumeric(const std::string& s)
{
    if (s.empty())
        return false;
    for (size_t i = 0; i < s.length(); ++i)
        if (!isdigit(s[i]))
            return false;
    return true;
}

std::string RequestValidator::matchedPrefix() const { return _matchedPrefix; }

const RouteConfig* RequestValidator::matchedRoute() const { return _matchedRoute; }

void RequestValidator::validate()
{
    matchRoute();  // Sets _matchedRoute (or NULL)
    validateMethod();
    validateHeaders();
    validateBody();
    // Add additional validation as needed
}

void RequestValidator::matchRoute()
{
    const std::map<std::string, RouteConfig>& routes = _serverConfig.routes;
    _matchedRoute                                    = NULL;
    _matchedPrefix                                   = "";

    for (std::map<std::string, RouteConfig>::const_iterator it = routes.begin(); it != routes.end();
         ++it)
    {
        const std::string& prefix = it->first;
        if (_req.path.compare(0, prefix.size(), prefix) == 0)
        {
            // Si ce prefix est plus long que le précédent, on retient
            if (prefix.size() > _matchedPrefix.size())
            {
                _matchedRoute  = &it->second;
                _matchedPrefix = prefix;
            }
        }
    }
    // Si aucune route ne matche, _matchedRoute reste NULL → fallback sur serverConfig
}

void RequestValidator::validateMethod()
{
    std::vector<std::string> allowed;
    if (_matchedRoute && _matchedRoute->allowedMethods.size() > 0)
        allowed = _matchedRoute->allowedMethods;
    else
        allowed = _serverConfig.allowedMethods;

    std::string method;
    switch (_req.method)
    {
        case HttpRequest::METHOD_GET:
            method = "GET";
            break;
        case HttpRequest::METHOD_POST:
            method = "POST";
            break;
        case HttpRequest::METHOD_DELETE:
            method = "DELETE";
            break;
        default:
            throw HttpException(400, "Bad Request: Unknown HTTP method", getErrorPage(400));
    }
    if (std::find(allowed.begin(), allowed.end(), method) == allowed.end())
        throw HttpException(405, "Method Not Allowed", getErrorPage(405));
}

void RequestValidator::validateBody()
{
    size_t max_size = _serverConfig.client_max_body_size;
    if (_matchedRoute)
    {
        if (_matchedRoute->client_max_body_size > 0)
            max_size = _matchedRoute->client_max_body_size;
    }
    if ((_req.method == HttpRequest::METHOD_POST) && _req.body.size() > max_size)
        throw HttpException(413, "Payload Too Large", getErrorPage(413));

    if ((_req.method == HttpRequest::METHOD_GET || _req.method == HttpRequest::METHOD_DELETE)
        && !_req.body.empty())
        throw HttpException(
            400, "Bad Request: Unexpected body in GET or DELETE request", getErrorPage(400));

    SVSMAP::const_iterator it = _req.headers.find("Content-Length");
    if (it != _req.headers.end() && !it->second.empty())
    {
        const std::string& val = it->second[0];
        if (isNumeric(val))
        {
            size_t declaredLength = static_cast<size_t>(strtol(val.c_str(), NULL, 10));
            if (_req.body.size() != declaredLength)
                throw HttpException(
                    400, "Bad Request: Body size does not match Content-Length", getErrorPage(400));
        }
    }
}

std::string RequestValidator::resolvePath(std::string relativePath, std::string root)
{
    std::string fullPath = root;

    if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/' && !relativePath.empty()
        && relativePath[0] != '/')
        fullPath += "/";

    fullPath += relativePath;
    return fullPath;
}

std::string RequestValidator::getErrorPage(int statusCode)
{
    // First, check route config (if matched)

    std::string path;

    if (_matchedRoute)
    {
        std::map<int, std::string>::const_iterator it = _matchedRoute->returnCodes.find(statusCode);
        if (it != _matchedRoute->returnCodes.end())
        {
            path = resolvePath(it->second, _matchedRoute->root);
            return path;
        }
    }
    // Then, check server config
    std::map<int, std::string>::const_iterator it2 = _serverConfig.error_pages.find(statusCode);
    if (it2 != _serverConfig.error_pages.end())
    {
        path = resolvePath(it2->second, _serverConfig.root);
        return path;
    }
    // No custom page found
    return "";
}

RouteConfig RequestValidator::getMatchedRoute() const { return *_matchedRoute; }

bool RequestValidator::hasMatchedRoute() const { return _matchedRoute != NULL; }

ServerConfig RequestValidator::getServerConfig() const { return _serverConfig; }

void RequestValidator::validateHeaders()
{
    size_t                 cl_count;
    size_t                 te_count;
    SVSMAP::const_iterator it;

    // 1. Host obligatoire et unique
    it = _req.headers.find("Host");
    if (it != _req.headers.end())
    {
        if (it->second.size() != 1)
            throw HttpException(400, "Bad Request: Multiple Host header", getErrorPage(400));
    }

    cl_count = _req.headers.count("Content-Length");
    if (cl_count > 1)
        throw HttpException(400, "Bad Request: Multiple Content-Length headers", getErrorPage(400));
    if (cl_count == 1)
    {
        const std::string& val = _req.headers.find("Content-Length")->second[0];
        if (!isNumeric(val))
            throw HttpException(400, "Bad Request: Invalid Content-Length", getErrorPage(400));
        if (strtol(val.c_str(), NULL, 10) < 0)
            throw HttpException(400, "Bad Request: Negative Content-Length", getErrorPage(400));
    }

    // 3. Transfer-Encoding : au max 1 fois, pas avec Content-Length
    te_count = _req.headers.count("Transfer-Encoding");
    if (te_count > 1)
        throw HttpException(
            400, "Bad Request: Multiple Transfer-Encoding headers", getErrorPage(400));
    if (cl_count == 1 && te_count == 1)
        throw HttpException(
            400,
            "Bad Request: Content-Length and Transfer-Encoding are mutually exclusive",
            getErrorPage(400));

    // 4. Interdit tous headers dupliqués sauf whitelist
    for (SVSMAP::const_iterator it_h = _req.headers.begin(); it_h != _req.headers.end(); ++it_h)
    {
        if (it_h->second.size() > 1)
        {
            if (it_h->first != "Cookie")
            {
                throw HttpException(400, "Bad Request: Duplicate header", getErrorPage(400));
            }
        }
    }
}
