/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:45:44 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/19 16:04:41 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>  // isspace, isdigit
#include <cerrno>  // errno
#include <cstdlib>
#include <cstring>  // strerror
#include "HttpException.hpp"
#include "HttpParser.hpp"
#include <cstdlib>  // pour strtol
#include <cctype>   // pour isxdigit

std::string urlDecode(const std::string& s)
{
    std::string out;
    size_t      i;
    char        hex[3];
    char        decoded;

    out.reserve(s.size());  // au moins la taille d'origine

    for (i = 0; i < s.size(); ++i)
    {
        if (s[i] == '%')
        {
            // doit avoir deux caractères hex suivant
            if (i + 2 >= s.size() || !std::isxdigit(static_cast<unsigned char>(s[i + 1]))
                || !std::isxdigit(static_cast<unsigned char>(s[i + 2])))
            {
                throw HttpException(400, "Bad Request");
            }

            // lire les deux hex, les convertir
            hex[0]  = s[i + 1];
            hex[1]  = s[i + 2];
            hex[2]  = '\0';
            decoded = static_cast<char>(std::strtol(hex, NULL, 16));
            out += decoded;
            i += 2;  // on a consommé %xx
        }
        else if (s[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += s[i];
        }
    }

    return out;
}


// découpe "a=b" en clé et valeur (URL-decode)
void splitKeyVal(const std::string& token, std::string& key, std::string& val)
{
    size_t pos;

    pos = token.find('=');
    if (pos == std::string::npos)
    {
        key = urlDecode(token);
        val.clear();
    }
    else
    {
        key = urlDecode(token.substr(0, pos));
        val = urlDecode(token.substr(pos + 1));
    }
    if (key.empty())
        throw HttpException(400, "Bad Request");
}

std::string trim(const std::string& s)
{
    size_t b;
    size_t e;

    b = 0, b = 0, e = s.size();
    while (b < e && isspace(s[b]))
        ++b;
    while (e > b && isspace(s[e - 1]))
        --e;
    return (s.substr(b, e - b));
}

bool containsCtl(const std::string& s)
{
    for (size_t i = 0; i < s.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(s[i]);
        // C0 controls (0x00–0x1F) et DEL (0x7F)
        if (c < 0x20 || c == 0x7F)
            return true;
    }
    return false;
}


bool pathEscapesRoot(const std::string& path)
{
    // On attend un chemin absolu commençant par '/'
    // Si ce n'est pas le cas, on considère ça comme invalide/évasion
    if (path.empty() || path[0] != '/')
        return true;

    std::vector<std::string> stack;
    std::string              segment;
    size_t                   i = 1;  // on skippe le '/' initial

    while (i <= path.size())
    {
        // extraire le segment jusqu'au prochain '/' ou fin
        size_t j = path.find('/', i);
        if (j == std::string::npos)
            j = path.size();

        segment.assign(path, i, j - i);

        if (segment == "..")
        {
            // on remonte d'un niveau : si on ne peut plus, c'est une évasion
            if (stack.empty())
                return true;
            stack.pop_back();
        }
        else if (!segment.empty() && segment != ".")
        {
            // un nom de dossier/fichier valide → on descend une branche
            stack.push_back(segment);
        }

        i = j + 1;
    }
    // si on n'a jamais débordé la racine, tout va bien
    return false;
}
