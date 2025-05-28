/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLimits.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 12:08:34 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/28 14:38:13 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPLIMITS_HPP
#define HTTPLIMITS_HPP

#include <cstdlib>

/*
 * These are **internal security limits** used by the HTTP parser.
 * They are based on practical limits from servers like Apache/nginx,
 * and protect against resource exhaustion (DoS, header bombing, etc.)
 *
 * These are **not influenced by server/user configuration** and must remain hard.
 * The user-configurable values (like client_max_body_size) are enforced later
 * by the RequestValidator.
 */


// Longueur maximale de la Request-Line (méthode + URI + version)
static const std::size_t MAX_URI_LEN = 8190;  // ~8 KiB, équivalent à LimitRequestLine 8190

// Nombre total maximum d’en-têtes
static const std::size_t MAX_HEADER_COUNT = 100;  // équivalent à LimitRequestFields 100

// Taille maximale d’un nom d’en-tête
static const std::size_t MAX_FIELD_NAME = 256;  // supporte “Content-Type”, “User-Agent”, etc.

// Taille maximale d’une valeur d’en-tête
static const std::size_t MAX_FIELD_VALUE = 8190;  // ~8 KiB, équivalent à LimitRequestFieldSize 8190

// Taille totale maximale de toutes les en-têtes combinées
static const std::size_t MAX_HEADERS_TOTAL_SIZE = 16384;  // 16 KiB, somme de toutes les lignes d’en-tête

// Taille maximale du corps (Content-Length)
static const std::size_t MAX_BODY_SIZE = 10485760;  // 10 MiB, équivalent à client_max_body_size 1m

// Taille maximale d’un chunk individuel en mode chunked
static const std::size_t MAX_CHUNK_SIZE = 65536;  // 64 KiB

// Longueur maximale de la query-string (après ‘?’)
static const std::size_t MAX_QUERY_LEN = 2048;  // ~2 048 caractères

// Nombre maximal de paires clés=valeurs dans query-string ou form-data
static const std::size_t MAX_PARAM_COUNT = 50;

// Taille maximale d’une clé ou valeur URL-encodée
static const std::size_t MAX_PARAM_LEN = 256;

#endif  // *************************************************** HTTPLIMITS_HPP //
