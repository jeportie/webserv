/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser_utils.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 16:10:53 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/19 16:11:18 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSER_UTILS_HPP
#define HTTPPARSER_UTILS_HPP

#include <string>
bool containsCtl(const std::string&);
std::string urlDecode(const std::string&);
void   splitKeyVal(const std::string&, std::string&, std::string&);
bool   pathEscapesRoot(const std::string&);
std::string trim(const std::string &s);


#endif
