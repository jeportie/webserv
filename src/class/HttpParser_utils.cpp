/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:45:44 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/15 17:36:34 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype> // isspace, isdigit
#include <cerrno> // errno
#include <cstdlib>
#include <cstring> // strerror
#include <iostream>
#include <sstream> // pour stringstream si besoin

// Utility : décodage %xx dans une chaîne
std::string urlDecode(const std::string &s)
{
	size_t	i;
	i = 0;
	char	buf[3] = {s[i + 1], s[i + 2], '\0'};
	char	decoded;

	std::string out;
	for (i = 0; i < s.size(); ++i)
	{
		if (s[i] == '%' && i + 2 < s.size() && isxdigit(s[i + 1])
			&& isxdigit(s[i + 2]))
		{
			decoded = static_cast<char>(strtol(buf, NULL, 16));
			out += decoded;
			i += 2;
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
	return (out);
}

// découpe "a=b" en clé et valeur (URL-decode)
void	splitKeyVal(const std::string &token, std::string &key,
		std::string &val)
{
	size_t	pos;

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
}

std::string trim(const std::string &s)
{
	size_t	b;
	size_t	e;

	b = 0, b = 0, e = s.size();
	while (b < e && isspace(s[b]))
		++b;
	while (e > b && isspace(s[e - 1]))
		--e;
	return (s.substr(b, e - b));
}
