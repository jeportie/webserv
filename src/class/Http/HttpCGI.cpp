/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpCGI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:10:34 by anastruc          #+#    #+#             */
/*   Updated: 2025/06/10 17:32:04 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ContentGenerator.hpp"
#include "HttpCGI.hpp"
#include "HttpException.hpp"
#include "HttpResponseBuilder.hpp"
#include "RequestValidator.hpp"
#include "ResponseFormatter.hpp"
#include "StatusUtils.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

std::vector<std::string> buildCgiEnv(const HttpRequest &request,
	const std::string &scriptPath)
{
	std::vector<std::string> env;
	std::ostringstream ss;
	// Ajout de tous les element de la httpRequest sous la forme de char* a l'env. On peut en ajouter
	// d'autre si besoin mais cela sont les elements requis par le norme pour lancer un process CGI
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + request.methodString());
	// Adapte à ta classe
	env.push_back("SCRIPT_FILENAME=" + scriptPath);
	env.push_back("SCRIPT_NAME=" + scriptPath);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	ss << request.body.size();
	env.push_back("CONTENT_LENGTH=" + ss.str());
	if (request.headers.count("Content-Type"))
		env.push_back("CONTENT_TYPE=" + request.headers.at("Content-Type")[0]);
	if (request.raw_query.length())
		env.push_back("QUERY_STRING=" + request.raw_query);
	// Ajout de tous les headers sous la forme HTTP_NAME=VAL, en bouclant sur la
	for (std::map<std::string, std::vector<std::string> >::const_iterator it = request.headers.begin(); it != request.headers.end(); ++it)
	{
		std::string key = it->first;
		std::string val;
		for (size_t j = 0; j < it->second.size(); ++j)
		{
			if (j != 0)
				val += ", ";
			val += it->second[j];
		}
		std::string http_key = "HTTP_";
		for (size_t i = 0; i < key.size(); ++i)
			http_key += (key[i] == '-' ? '_' : toupper(key[i]));
		env.push_back(http_key + "=" + val);
	}
	return (env);
}

char	**vectorToEnv(const std::vector<std::string> &env)
{
	char	**envp;

	envp = new char *[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
	{
		envp[i] = strdup(env[i].c_str());
	}
	envp[env.size()] = NULL;
	return (envp);
}
bool	waitWithTimeout(pid_t pid, int timeout_secs, int &status)
{
	time_t	start;
	pid_t	result;

	start = time(NULL);
	while (true)
	{
		result = waitpid(pid, &status, WNOHANG);
		if (result == pid)
			return (true); // process terminé
		if (result == -1)
			return (false); // erreur waitpid
		if (time(NULL) - start >= timeout_secs)
		{
			kill(pid, SIGKILL);       // tue le process CGI
			waitpid(pid, NULL, 0);    // évite un zombie
			return (false); // timeout
		}
		usleep(1000); // 1 ms pour ne pas boucler comme un porc
	}
}

std::string HttpResponseBuilder::runCgiScript(HttpRequest &request,
	const std::string &interpreterPath, RequestValidator &_validator)
{
	int pipe_in[2], pipe_out[2];
	pid_t pid;
	char **envp;
	char *argv[3];
	char buffer[4096];
	ssize_t bytes_read;
	int status;
	std::string scriptPath = resolveTargetPath();
	std::cout << "CGI USE : " << scriptPath << std::endl;
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		throw HttpException(500, "Internal server error : pipe() failed",
			_validator.getErrorPage(500));
	pid = fork();
	if (pid < 0)
		throw HttpException(500, "Internal server error : fork() failed",
			_validator.getErrorPage(500));
	else if (pid == 0)
	{ // Child process
		// Préparation de l'environnement
		std::vector<std::string> env = buildCgiEnv(request, interpreterPath);
		envp = vectorToEnv(env);
		// Redirection stdin et stdout
		close(pipe_in[1]);
		close(pipe_out[0]);
		if (pipe_in[0] != STDIN_FILENO)
		{
			dup2(pipe_in[0], STDIN_FILENO);
			close(pipe_in[0]);
		}
		if (pipe_out[1] != STDOUT_FILENO)
		{
			dup2(pipe_out[1], STDOUT_FILENO);
			close(pipe_out[1]);
		}
		// Prépare les arguments pour execve
		argv[0] = strdup(interpreterPath.c_str());
		argv[1] = strdup(scriptPath.c_str()); // findscript
		argv[2] = NULL;
		execve(interpreterPath.c_str(), argv, envp);
		// Si exec échoue :
		throw HttpException(500, "Internal server error : execve()",
			_validator.getErrorPage(500));
	}
	else
	{ // Parent process
		close(pipe_in[0]);
		close(pipe_out[1]);
		// Si POST, envoie le body de la requête sur stdin du CGI
		ssize_t written;
		if (request.method == HttpRequest::METHOD_POST && !request.body.empty())
		{
			written = write(pipe_in[1], request.body.c_str(),
					request.body.size());
			if (written != (ssize_t)request.body.size())
				throw HttpException(500,
					"Internal server error : Write in children process failed",
					_validator.getErrorPage(500));
		}
		close(pipe_in[1]); // Très important, sinon CGI peut rester bloqué
		status = 0;
		bool ok = waitWithTimeout(pid, 5, status);
		// Lecture de la sortie du CGI
		std::string output;
		while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
		{
			output.append(buffer, bytes_read);
		}
		close(pipe_out[0]);
		if (!ok)
			throw HttpException(504, "Gateway Timeout",
				_validator.getErrorPage(504));
		// Option : vérifier le code de retour, détecter erreurs du script...
		return (output);
	}
}