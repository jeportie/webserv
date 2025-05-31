/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 11:14:39 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/31 15:02:57 by fsalomon         ###   ########.fr       */
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
    HttpException(int status, const std::string &reason, const std::string &customPage);
    ~HttpException() throw();

    int status() const;
    std::string customPage() const;

    // raison (sera exposée en Content-Type: text/plain)
    const char *what() const throw();

private:
    int _status;
    std::string _reason;
    std::string _customPage; // chemin de la page d'erreur custom, vide si aucune
};

void sendErrorResponse(int fd, int status, const std::string &reason);
void sendCustomErrorResponse(int fd, int status, const std::string& errorPagePath);



#endif // ************************************************ HTTPEXCEPTION_HPP //
