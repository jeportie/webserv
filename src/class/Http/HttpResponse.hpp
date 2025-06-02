/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:40:19 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:02:49 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse
{
private:
    int                                _statusCode;
    std::string                        _statusMessage;
    std::map<std::string, std::string> _headers;
    std::string                        _body;

public:
    HttpResponse();
    ~HttpResponse();
    void setStatus(int code, const std::string& message);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& content);

    int                                       getStatusCode() const;
    std::string                               getStatusMessage() const;
    std::string                               getHeader(const std::string& key) const;
    std::string                               getBody() const;
    const std::map<std::string, std::string>& getAllHeaders() const;
};

#endif
