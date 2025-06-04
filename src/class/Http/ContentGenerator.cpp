/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentGenerator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 13:22:41 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/04 12:56:12 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ContentGenerator.hpp"
#include "StatusUtils.hpp"
#include <map>
#include <sys/stat.h>  // Pour struct stat
#include <fstream>
#include <sstream>
#include <cstddef>  // std::size_t
#include <string>
#include <dirent.h>

bool fileExists(const std::string& path)
{
    struct stat fileStat;

    if (stat(path.c_str(), &fileStat) != 0)
        return false;

    return S_ISREG(fileStat.st_mode);
}

bool isDirectory(const std::string& path)
{
    struct stat dirStat;
    return (stat(path.c_str(), &dirStat) == 0 && S_ISDIR(dirStat.st_mode));
}

bool isExecutable(const std::string& path)
{
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0 && (statbuf.st_mode & S_IXUSR));
}

bool readFileContent(const std::string& path, std::string& out)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open())
        return false;

    std::ostringstream ss;
    ss << file.rdbuf();  // Lit tout le contenu
    out = ss.str();
    file.close();

    return true;
}

// Fonction pour obtenir le type MIME à partir de l'extension du fichier
// std::string mime = getMimeType("/www/index.html");
// → "text/html"
// std::string mime2 = getMimeType("image.unknown");
// → "application/octet-stream"
std::string getMimeType(const std::string& path)
{
    std::map<std::string, std::string> mimeTypes;

    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"]  = "text/html";
    mimeTypes[".css"]  = "text/css";
    mimeTypes[".js"]   = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".png"]  = "image/png";
    mimeTypes[".jpg"]  = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"]  = "image/gif";
    mimeTypes[".svg"]  = "image/svg+xml";
    mimeTypes[".ico"]  = "image/x-icon";
    mimeTypes[".txt"]  = "text/plain";
    mimeTypes[".pdf"]  = "application/pdf";
    mimeTypes[".zip"]  = "application/zip";

    // Extraire l’extension à partir du nom de fichier
    std::size_t dot = path.rfind('.');
    if (dot != std::string::npos)
    {
        std::string                                        ext = path.substr(dot);
        std::map<std::string, std::string>::const_iterator it  = mimeTypes.find(ext);
        if (it != mimeTypes.end())
            return it->second;
    }

    return "application/octet-stream";  // valeur par défaut
}


// Résout un chemin relatif par rapport à root
std::string resolvePath(const std::string& relativePath, const std::string& root)
{
    if (relativePath.empty())
        return "";

    // Si le chemin est déjà absolu (commence par '/'), on le retourne tel quel
    if (relativePath[0] == '/')
        return relativePath;

    // Sinon on concatène root + "/" + relativePath
    if (!root.empty() && root[root.size() - 1] == '/')
        return root + relativePath;
    else
        return root + "/" + relativePath;
}

// Génère la page d'erreur, si le fichier errorPagePath existe et est lisible, retourne son contenu
// sinon retourne une page HTML générée par défaut
std::string generateErrorPage(int code, const std::string& errorPagePath)
{
    std::string content;

    if (!errorPagePath.empty())
    {
        if (readFileContent(errorPagePath, content))
            return content;
        // Sinon on continue et génère la page par défaut
    }

    std::ostringstream ss;
    std::string        message = getStatusMessage(code);

    ss << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "<head><title>" << code << " " << message << "</title></head>\n"
       << "<body>\n"
       << "<h1>" << code << " " << message << "</h1>\n"
       << "<p>The server encountered an error.</p>\n"
       << "</body>\n"
       << "</html>\n";

    return ss.str();
}

std::string generateAutoIndexPage(const std::string& path, const std::string& uri)
{
    std::ostringstream oss;
    oss << "<html><head><title>Index of " << uri << "</title></head><body>";
    oss << "<h1>Index of " << uri << "</h1>";
    oss << "<ul>";

    // Ajout du lien vers le dossier parent ".."
    if (uri != "/")
    {
        std::string parent = uri;
        if (parent[parent.size() - 1] == '/')
            parent = parent.substr(0, parent.size() - 1);
        size_t pos = parent.find_last_of('/');
        if (pos != std::string::npos)
            parent = parent.substr(0, pos + 1);
        else
            parent = "/";
        oss << "<li><a href=\"" << parent << "\">..</a></li>";
    }

    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
    {
        oss << "<li>Unable to open directory</li>";
    }
    else
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            std::string name = entry->d_name;
            if (name == "." || name == "..")
                continue;

            std::string fullPath = path;
            if (fullPath[fullPath.size() - 1] != '/')
                fullPath += "/";
            fullPath += name;

            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0)
            {
                if (S_ISDIR(st.st_mode))
                    name += "/";
            }

            oss << "<li><a href=\"" << uri;
            if (uri[uri.size() - 1] != '/')
                oss << "/";
            oss << name << "\">" << name << "</a></li>";
        }
        closedir(dir);
    }

    oss << "</ul>";
    oss << "<hr><address>MyWebserv autoindex</address>";
    oss << "</body></html>";

    return oss.str();
}
