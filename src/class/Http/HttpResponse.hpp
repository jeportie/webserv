/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:40:19 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/03 12:54:01 by anastruc         ###   ########.fr       */
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

    int getStatusCode() const;
    std::string getStatusMessage() const;
    std::string getHeader(const std::string &key) const;
    std::string getBody() const;
    const std::map<std::string, std::string> &getAllHeaders() const;
    void parseCgiOutputAndSet(const std::string& cgi_output);
    bool hasHeader(const std::string& key) const;


};

#endif
