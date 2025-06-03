/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpCGI.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:11:43 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/02 18:12:03 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCGI_HPP
#define HTTPCGI_HPP

#include "HttpException.hpp"
#include "HttpRequest.hpp"

std::vector<std::string> buildCgiEnv(const HttpRequest &request,
	const std::string &scriptPath);

char	**vectorToEnv(const std::vector<std::string> &env);

std::string runCgiScript(HttpRequest &request, const std::string &scriptPath);

bool waitWithTimeout(pid_t pid, int timeout_secs, int &status);


#endif ;
