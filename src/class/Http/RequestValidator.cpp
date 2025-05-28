/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 15:13:38 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/28 17:34:15 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestValidator.hpp"
#include <algorithm>
#include <set>

RequestValidator::RequestValidator(const HttpRequest& req, const ServerConfig& serverConfig)
: _req(req),
_serverConfig(serverConfig) {}

RequestValidator::~RequestValidator() {}

void RequestValidator::validate() {
    matchRoute();       // Sets _matchedRoute (or NULL)
    validateMethod();
    validateHeaders();
    validateBody();
    // Add additional validation as needed
}

void RequestValidator::matchRoute() {
    const std::map<std::string, RouteConfig>& routes = _serverConfig.routes;
    _matchedRoute = NULL;
    _matchedPrefix = "";
    
    for (std::map<std::string, RouteConfig>::const_iterator it = routes.begin();
    it != routes.end(); ++it) {
        const std::string& prefix = it->first;
        if (_req.path.compare(0, prefix.size(), prefix) == 0) {
            // Si ce prefix est plus long que le précédent, on retient
            if (prefix.size() > _matchedPrefix.size()) {
                _matchedRoute = &it->second;
                _matchedPrefix = prefix;
            }
        }
    }
    // Si aucune route ne matche, _matchedRoute reste NULL → fallback sur serverConfig
}

void RequestValidator::validateMethod()
{
    std::vector<std::string> allowed;
    if (_matchedRoute)
    allowed = _matchedRoute->allowedMethods;
    else
    allowed = _serverConfig.allowedMethods;
    
    std::string method;
    switch (_req.method) {
        case HttpRequest::METHOD_GET: method = "GET"; break;
        case HttpRequest::METHOD_POST: method = "POST"; break;
        case HttpRequest::METHOD_DELETE: method = "DELETE"; break;
        default: throw HttpException(400, "Unknown HTTP method");
    }
    if (std::find(allowed.begin(), allowed.end(), method) == allowed.end())
    throw HttpException(405, "Method Not Allowed");
}

void RequestValidator::validateBody() 
{
    size_t max_size = _serverConfig.client_max_body_size;
    if (_matchedRoute && _matchedRoute->client_max_body_size > 0)
    max_size = _matchedRoute->client_max_body_size;
    
    if ((_req.method == HttpRequest::METHOD_POST)
    && _req.body.size() > max_size)
    throw HttpException(413, "Payload Too Large");
    
    if ((_req.method == HttpRequest::METHOD_GET || _req.method == HttpRequest::METHOD_DELETE)
    && !_req.body.empty())
    throw HttpException(400, "GET/DELETE should not have a body");
}

// 5. Retrieve error page path for a given status code
std::string RequestValidator::getErrorPage(int statusCode) const {
    // First, check route config (if matched)
    if (_matchedRoute) {
        std::map<int, std::string>::const_iterator it = _matchedRoute->returnCodes.find(statusCode);
        if (it != _matchedRoute->returnCodes.end())
        return it->second;
    }
    // Then, check server config
    std::map<int, std::string>::const_iterator it2 = _serverConfig.error_pages.find(statusCode);
    if (it2 != _serverConfig.error_pages.end())
    return it2->second;
    // No custom page found
    return "";
}


// Helper : vérifie si une chaîne est un entier non signé (numérique)
static bool isNumeric(const std::string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.length(); ++i)
        if (!isdigit(s[i]))
            return false;
    return true;
}


static const std::set<std::string> kDuplicableHeaders = {
    "Cookie"
    // Ajoute ici d'autres headers si besoin, selon RFC
};

void RequestValidator::validateHeaders() {
    // 1. Host obligatoire et unique
    SVSMAP::const_iterator it = _req.headers.find("Host");
    if (it == _req.headers.end())
        throw HttpException(400, "Missing Host header");
    if (it->second.size() != 1)
        throw HttpException(400, "Multiple Host header");

    // 2. Content-Length : au max 1 fois, valeur numérique >=0, pas avec Transfer-Encoding
    size_t cl_count = _req.headers.count("Content-Length");
    if (cl_count > 1)
        throw HttpException(400, "Multiple Content-Length headers");
    if (cl_count == 1) {
        const std::string& val = _req.headers.find("Content-Length")->second[0];
        if (!isNumeric(val))
            throw HttpException(400, "Invalid Content-Length");
        if (strtol(val.c_str(), NULL, 10) < 0)
            throw HttpException(400, "Negative Content-Length");
    }

    // 3. Transfer-Encoding : au max 1 fois, pas avec Content-Length
    size_t te_count = _req.headers.count("Transfer-Encoding");
    if (te_count > 1)
        throw HttpException(400, "Multiple Transfer-Encoding headers");
    if (cl_count == 1 && te_count == 1)
        throw HttpException(400, "Content-Length and Transfer-Encoding are mutually exclusive");

    // 4. Interdit tous headers dupliqués sauf whitelist
    for (SVSMAP::const_iterator it = _req.headers.begin(); it != _req.headers.end(); ++it) {
        if (it->second.size() > 1) {
            if (it->first != "Host" && it->first != "Content-Length" && it->first != "Transfer-Encoding"
                && kDuplicableHeaders.find(it->first) == kDuplicableHeaders.end())
            {
                throw HttpException(400, "Duplicate header: " + it->first);
            }
        }
    }
    // Ajoute ici d’autres validations spécifiques si besoin
}