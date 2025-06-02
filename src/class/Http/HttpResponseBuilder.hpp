/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:49:19 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:03:34 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEBUILDER_HPP
#define HTTPRESPONSEBUILDER_HPP

#include "HttpResponse.hpp"
#include <string>
#include "HttpRequest.hpp"
#include "RequestValidator.hpp"

class HttpResponseBuilder
{
private:
    HttpRequest      _request;
    RequestValidator _validator;
    HttpResponse     _response;

public:
    HttpResponseBuilder(const HttpRequest& request, const RequestValidator& validator);

    void buildResponse();  // Point d'entrée principal

    const HttpResponse& getResponse() const;

private:
    void handleGET();
    void handlePOST();
    void handleDELETE();

    std::string resolveTargetPath();  // Trouve le fichier cible (root/index/etc)
    void        setConnection();
    std::string runCgiScript(HttpRequest& request, const std::string& scriptPath);     // TO DO
    bool storeUploadedFile(HttpRequest& request, const std::string& uploadStorePath);  // TO DO
};

#endif
