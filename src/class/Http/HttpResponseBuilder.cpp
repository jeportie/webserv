/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:00:11 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 18:37:18 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ContentGenerator.hpp"
#include "HttpCGI.hpp"
#include "HttpException.hpp"
#include "HttpResponseBuilder.hpp"
#include "RequestValidator.hpp"
#include "ResponseFormatter.hpp"
#include "StatusUtils.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Constructeur
HttpResponseBuilder::HttpResponseBuilder(const HttpRequest &req,
	const RequestValidator &validator) : _request(req), _validator(validator),
	_response()
{
}

std::string toString(size_t value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

// Point d’entrée principal
void HttpResponseBuilder::buildResponse()
{
	switch (_request.method)
	{
	case HttpRequest::METHOD_GET:
		handleGET();
		break ;
	case HttpRequest::METHOD_POST:
		handlePOST();
		break ;
	case HttpRequest::METHOD_DELETE:
		handleDELETE();
		break ;
	default:
		throw HttpException(405, "Method Not Allowed",
			_validator.getErrorPage(405));
		break ;
	}
}

// Accès à la réponse finale
const HttpResponse &HttpResponseBuilder::getResponse() const
{
	return (_response);
}

// Gestion GET
void HttpResponseBuilder::handleGET()
{
    // rajout du bloc d'executation du CGi si presence d'un CGI, le CGI prime sur le reste
	if (_validator.hasMatchedRoute())
	{
		const RouteConfig &route = _validator.getMatchedRoute();
		if (!route.cgiExecutor.first.empty())
		{
			// Si la requête cible le script, on exécute le CGI
			if (!isExecutable(route.cgiExecutor.second))
				throw HttpException(403, "Forbidden",
					_validator.getErrorPage(403));
			std::string output = runCgiScript(_request,
					route.cgiExecutor.second, _validator);
			_response.setStatus(200, "OK");
            _response.parseCgiOutputAndSet(output);
			setConnection();
			return ;
		}
	}
	std::string path = resolveTargetPath();
	// Si le chemin est un répertoire sans index
	if (isDirectory(path))
	{
		if (!_validator.hasMatchedRoute())
		{
			// Pas de route correspondante, refuse l'accès ou 404
			throw HttpException(404, "Not Found", _validator.getErrorPage(404));
		}
		const RouteConfig &route = _validator.getMatchedRoute();
		if (route.autoindex)
		{
			std::string html = generateAutoIndexPage(path, _request.path);
			_response.setStatus(200, "OK");
			_response.setHeader("Content-Type", "text/html");
			_response.setHeader("Content-Length", toString(html.size()));
			_response.setBody(html);
		}
		else
		{
			throw HttpException(403, "Forbidden", _validator.getErrorPage(403));
		}
		return ;
	}
	// Sinon, comportement standard
	if (!fileExists(path))
		throw HttpException(404, "Not Found", _validator.getErrorPage(404));
	std::string content;
	if (!readFileContent(path, content))
		throw HttpException(500, "Internal Server Error",
			_validator.getErrorPage(500));
	std::string mimeType = getMimeType(path);
	_response.setStatus(200, "OK");
	_response.setHeader("Content-Type", mimeType);
	_response.setHeader("Content-Length", toString(content.size()));
	_response.setBody(content);
	setConnection();
}

// Gestion POST (exemple simple : echo ou cgi)
void HttpResponseBuilder::handlePOST()
{
	bool	success;

	if (_validator.hasMatchedRoute())
	{
		const RouteConfig &route = _validator.getMatchedRoute();
		if (!route.cgiExecutor.first.empty())
		{
			if (!isExecutable(route.cgiExecutor.second))
				throw HttpException(403, "Forbidden",
					_validator.getErrorPage(403));
			std::string output = runCgiScript(_request,
					route.cgiExecutor.second, _validator);
			_response.setStatus(200, "OK");
			_response.parseCgiOutputAndSet(output);
		}
		else if (route.uploadEnabled && !route.uploadStore.empty())
		{
			// Upload activé sur la route
			success = storeUploadedFile(_request, route.uploadStore);
			if (success)
			{
				_response.setStatus(201, "Created");
				_response.setBody("File uploaded successfully.");
			}
			else
			{
				throw HttpException(500, "Internal Server Error",
					_validator.getErrorPage(500));
			}
		}
		else
		{
			// Pas de CGI : renvoyer le corps brut
			_response.setStatus(200, "OK");
			_response.setHeader("Content-Type", "text/plain");
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

// fonction pour supprimer le fichier
bool	deleteFile(const std::string &path)
{
	// std::remove retourne 0 si succès
	return (std::remove(path.c_str()) == 0);
}

// Gestion DELETE (simplifiée)
void HttpResponseBuilder::handleDELETE()
{
	std::string path = resolveTargetPath();
	if (!fileExists(path))
	{
		throw HttpException(404, "Not Found", _validator.getErrorPage(404));
	}
	if (!deleteFile(path))
	{
		throw HttpException(500, "Internal Server Error",
			_validator.getErrorPage(500));
	}
	// Réponse 204 No Content est classique pour un DELETE réussi sans corps de réponse
	_response.setStatus(204, "No Content");
	// Si tu veux renvoyer 200 OK avec un message, décommente ces lignes:
	// _response.setStatus(200, "OK");
	// _response.setHeader("Content-Type", "text/plain");
	// _response.setBody("File deleted successfully.");
	setConnection();
}

std::string HttpResponseBuilder::resolveTargetPath()
{
	bool	indexIsSet;

	std::string root;
	std::vector<std::string> indexFiles;
	indexIsSet = false;
	std::string defaultFile;
	std::string uri = _request.path;
	if (_validator.hasMatchedRoute())
	{
		const RouteConfig &route = _validator.getMatchedRoute();
		root = route.root;
		indexFiles = route.indexFiles;
		indexIsSet = route.indexIsSet;
		defaultFile = route.defaultFile;
	}
	else
	{
		const ServerConfig &serverConf = _validator.getServerConfig();
		root = serverConf.root;
		indexFiles = serverConf.indexFiles;
		indexIsSet = serverConf.indexIsSet;
		defaultFile = "index.html"; // Fallback
	}
	// Construction du chemin complet
	if (!root.empty() && root[root.size() - 1] != '/' && uri[0] != '/')
		root += "/";
	std::string fullPath = root + uri;
	if (isDirectory(fullPath))
	{
		if (fullPath[fullPath.size() - 1] != '/')
			fullPath += "/";
		// Si des fichiers index ont été définis explicitement
		if (indexIsSet)
		{
			for (std::vector<std::string>::iterator it = indexFiles.begin(); it != indexFiles.end(); ++it)
			{
				std::string candidate = fullPath + *it;
				if (fileExists(candidate))
					return (candidate);
			}
			// Aucun fichier trouvé → on retourne le dossier brut (autoindex ?)
			return (fullPath);
		}
		// Sinon on peut fallback sur defaultFile (même si index non set)
		std::string fallback = fullPath + defaultFile;
		if (!defaultFile.empty() && fileExists(fallback))
			return (fallback);
		// Aucun fichier d’index trouvé, retour du dossier
		return (fullPath);
	}
	return (fullPath);
}

void HttpResponseBuilder::setConnection()
{
	std::map<std::string,
		std::vector<std::string>>::const_iterator it = _request.headers.find("Connection");
	if (it == _request.headers.end())
	{
		// Header "Connection" absent dans la requête
		if (_request.http_major == 1 && _request.http_minor == 0)
		{
			// HTTP/1.0 par défaut : connexion close
			_response.setHeader("Connection", "close");
		}
		else
		{
			// HTTP/1.1 et plus : connexion keep-alive par défaut
			_response.setHeader("Connection", "keep-alive");
		}
	}
	else
	{
		// Header "Connection" présent, vérifie la valeur
		std::string connection = "";
		if (!it->second.empty())
			connection = it->second[0];
		if (connection == "keep-alive")
			_response.setHeader("Connection", "keep-alive");
		else
			_response.setHeader("Connection", "close");
	}
}

// ... autres includes si nécessaire

bool HttpResponseBuilder::storeUploadedFile(HttpRequest &request,
	const std::string &uploadStorePath)
{
	(void)request;         // Pour éviter l'avertissement non utilisé
	(void)uploadStorePath; // Pour éviter l'avertissement non utilisé
	return (true);
	// TO DO
	/* Comportement attendu

	Analyser la requête pour extraire le fichier envoyé

		En fonction du type de contenu (généralement multipart/form-data),
			il faut parser le corps pour récupérer le contenu du fichier et son nom.

	Vérifier la validité du dossier de destination

		S’assurer que le dossier uploadStore existe et est accessible en écriture.

	Créer un nouveau fichier dans uploadStore

		Le nom du fichier peut être récupéré depuis la requête ou généré (éviter collisions).

		Ouvrir ce fichier en écriture binaire.

	Écrire les données du fichier extrait dans ce fichier

		Copier le contenu du fichier uploadé dans le fichier cible.

	Gérer les erreurs (dossiers non accessibles, problèmes écriture...)

	Retourner true si l’opération réussit, false sinon

Exemple de tâches concrètes dans storeUploadedFile

	Parser _request.body pour extraire le fichier (nom et contenu).

	Vérifier que route.uploadStore existe (créer si nécessaire).

	Construire un chemin complet (ex : uploadStore/nomFichier).

	Ouvrir et écrire dans ce fichier.

	Fermer le fichier.

	Retourner true/false.

Attention

	Si le parsing multipart/form-data n’est pas encore fait,
		il faudra l’implémenter ou s’assurer que _request.form_data contient déjà le fichier et ses données.

	La fonction peut aussi gérer la limitation de taille,
		le contrôle des extensions, etc. selon ta config. */
}