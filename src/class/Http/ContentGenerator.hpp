/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentGenerator.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 12:50:02 by fsalomon          #+#    #+#             */
/*   Updated: 2025/06/02 17:47:04 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTENTGENERATOR_HPP
#define CONTENTGENERATOR_HPP

#include <string>

bool fileExists(const std::string& path);
bool isDirectory(const std::string& path);
bool isExecutable(const std::string& path);
bool readFileContent(const std::string& path, std::string& out);
std::string getMimeType(const std::string& path);

std::string resolvePath(const std::string& relativePath, const std::string& root);
std::string generateErrorPage(int code, const std::string& errorPagePath);
std::string generateAutoIndexPage(const std::string& path, const std::string& uri);

#endif
