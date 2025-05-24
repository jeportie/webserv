/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:14:39 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/19 15:27:20 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPEXCEPTION_HPP
#define HTTPEXCEPTION_HPP

#define HTTPEXC_CONST "HttpException constructor called"
#define HTTPEXC_DEST "HttpException destructor called"

#include <exception>
#include <string>

class HttpException : public std::exception
{
public:
    HttpException(int status, const std::string& reason);
    ~HttpException() throw();

    int status() const;
    // raison (sera exposée en Content-Type: text/plain)
    const char* what() const throw();

private:
    int         _status;
    std::string _reason;
};

void sendErrorResponse(int fd, int status, const std::string& reason);

#endif  // ************************************************ HTTPEXCEPTION_HPP //
