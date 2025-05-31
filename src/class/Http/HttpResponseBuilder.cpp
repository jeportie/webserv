/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:00:11 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/31 17:13:10 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponseBuilder.hpp"
#include "HttpException.hpp"
#include "ResponseFormatter.hpp"
#include "ContentGenerator.hpp"
#include "StatusUtils.hpp"
#include "RequestValidator.hpp"
#include <sstream>

// Constructeur
HttpResponseBuilder::HttpResponseBuilder(const HttpRequest &req, const RequestValidator &validator)
    : _request(req), _validator(validator), _response()
{
}

std::string toString(size_t value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Point d’entrée principal
void HttpResponseBuilder::buildResponse()
{

    switch (_request.method)
    {
    case HttpRequest::METHOD_GET:
        handleGET();
        break;
    case HttpRequest::METHOD_POST:
        handlePOST();
        break;
    case HttpRequest::METHOD_DELETE:
        handleDELETE();
        break;
    default:
        throw HttpException(405, "Method Not Allowed", _validator.getErrorPage(405));
        break;
    }
}

// Accès à la réponse finale
const HttpResponse &HttpResponseBuilder::getResponse() const
{
    return _response;
}

// Gestion GET
void HttpResponseBuilder::handleGET()
{
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
        return;
    }

    // Sinon, comportement standard
    if (!fileExists(path))
        throw HttpException(404, "Not Found", _validator.getErrorPage(404));

    std::string content;
    if (!readFileContent(path, content))
        throw HttpException(500, "Internal Server Error", _validator.getErrorPage(500));

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
    if (_validator.hasMatchedRoute())
    {
        const RouteConfig &route = _validator.getMatchedRoute();

        if (!route.cgiExecutor.first.empty())
        {
            // Route CGI configurée : exécuter script
            std::string output = runCgiScript(_request, route.cgiExecutor.second);
            _response.setStatus(200, "OK");
            _response.setHeader("Content-Type", "text/html");
            _response.setBody(output);
        }
        else if (route.uploadEnabled && !route.uploadStore.empty())
        {
            // Upload activé sur la route
            bool success = storeUploadedFile(_request, route.uploadStore);
            if (success)
            {
                _response.setStatus(201, "Created");
                _response.setBody("File uploaded successfully.");
            }
            else
            {
                throw HttpException(500, "Internal Server Error", _validator.getErrorPage(500));
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
bool deleteFile(const std::string &path)
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
        throw HttpException(500, "Internal Server Error", _validator.getErrorPage(500));
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
    std::string root;
    std::vector<std::string> indexFiles;
    bool indexIsSet = false;
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
                    return candidate;
            }
            // Aucun fichier trouvé → on retourne le dossier brut (autoindex ?)
            return fullPath;
        }

        // Sinon on peut fallback sur defaultFile (même si index non set)
        std::string fallback = fullPath + defaultFile;
        if (!defaultFile.empty() && fileExists(fallback))
            return fallback;

        // Aucun fichier d’index trouvé, retour du dossier
        return fullPath;
    }

    return fullPath;
}


void HttpResponseBuilder::setConnection()
{
    std::map<std::string, std::vector<std::string> >::const_iterator it = _request.headers.find("Connection");

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

std::string HttpResponseBuilder::runCgiScript(HttpRequest &request, const std::string &scriptPath)
{
    (void)request; // Pour éviter l'avertissement non utilisé
    (void)scriptPath; // Pour éviter l'avertissement non utilisé
    return "";
    // TO DO 
    /*Comportement attendu

    Préparer l’environnement d’exécution CGI

        Initialiser les variables d’environnement nécessaires (ex : REQUEST_METHOD, QUERY_STRING, CONTENT_LENGTH, CONTENT_TYPE, SCRIPT_NAME, etc.) selon la requête HTTP et la spécification CGI.

    Créer un processus fils (fork + exec)

        Exécuter le script CGI indiqué (route.cgiExecutor.second représente généralement le chemin du script).

        Passer l’entrée standard du script (pour POST, le corps de la requête) via un pipe.

        Récupérer la sortie standard du script via un autre pipe.

    Écrire dans le pipe l’éventuel contenu POST

        Envoyer le corps de la requête au script via son entrée standard.

    Lire la sortie du script

        Lire la sortie du script depuis le pipe de sortie standard.

        Cette sortie inclut généralement les headers HTTP (Content-Type, etc.) suivis d’une ligne vide puis du corps.

    Fermer les pipes et attendre la fin du processus fils

        S’assurer que tout est proprement nettoyé.

    Retourner la sortie complète du script sous forme de string

        Le contenu retourné sera ensuite injecté dans le corps de la réponse HTTP.

Points importants

    La fonction doit gérer les erreurs (échec fork, exec, timeout, script non trouvé).

    Le script CGI doit respecter la norme CGI (produire des headers HTTP valides avant le contenu).

    Les variables d’environnement doivent être configurées avec soin pour que le script reçoive toutes les infos nécessaires.

    Penser à gérer les permissions d’exécution du script.

Exemple des étapes internes (conceptuelles) de runCgiScript

    Préparer variables d’environnement CGI à partir de _request.

    Créer 2 pipes : un pour envoyer la requête POST au script, un autre pour lire la sortie.

    fork() un processus fils.

    Dans le fils :

        Rediriger stdin et stdout vers les pipes.

        execve() du script CGI.

    Dans le père :

        Envoyer le corps de la requête via pipe entrée (si POST).

        Lire la sortie complète du script via pipe sortie.

        Attendre la fin du fils.

    Retourner la sortie récupérée sous forme de string.*/
}


bool HttpResponseBuilder::storeUploadedFile(HttpRequest &request, const std::string &uploadStorePath)
{
    (void)request; // Pour éviter l'avertissement non utilisé
    (void)uploadStorePath; // Pour éviter l'avertissement non utilisé
    return true;
    // TO DO 
    /* Comportement attendu

    Analyser la requête pour extraire le fichier envoyé

        En fonction du type de contenu (généralement multipart/form-data), il faut parser le corps pour récupérer le contenu du fichier et son nom.

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

    Si le parsing multipart/form-data n’est pas encore fait, il faudra l’implémenter ou s’assurer que _request.form_data contient déjà le fichier et ses données.

    La fonction peut aussi gérer la limitation de taille, le contrôle des extensions, etc. selon ta config. */
}