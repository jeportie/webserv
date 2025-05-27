/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.api.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 09:31:48 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 12:22:34 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

bool Lexer::isAtEnd() const {
    return _pos >= _content.size();
}

void Lexer::skipWhitespace()
{
    if (!isAtEnd() && isspace(peek()))
        get();
}

void Lexer::skipComment()
{
    if (peek() == '#')
    { 
        while (!isAtEnd() && get() != '\n')
        {
            //consume until end of line
        }
    }
        
}

bool Lexer::isValidIdentifierSymbol(char c) const
{
    if (c == '_' || c == '.' || c == '/' || c == '-')
        return true;
    return false;
}

Token Lexer::parseIdentifier()
{
    std::string value;
    int startLine = _line;
    int startColumn = _column;


    while (!isAtEnd() && (std::isalnum(peek()) || isValidIdentifierSymbol(peek())))
    {
        value += get();
    }
    return Token(TOKEN_IDENTIFIER, value, startLine, startColumn);
}

bool Lexer::isNextTokenUrl() const
{
    size_t tmpPos = _pos;
    std::string protocol;

    // Lire les lettres pour voir si c'est "http" ou "https"
    while (tmpPos < _content.size() && std::isalpha(_content[tmpPos]))
        protocol += _content[tmpPos++];

    if (protocol != "http" && protocol != "https")
        return false;

    // Vérifie que c'est bien suivi de "://"
    return tmpPos + 2 < _content.size()
        && _content[tmpPos] == ':'
        && _content[tmpPos + 1] == '/'
        && _content[tmpPos + 2] == '/';
}

Token Lexer::parseUrl()
{
    int startLine = _line;
    int startColumn = _column;
    std::string value;

    while (!isAtEnd())
    {
        char c = peek();
        if (std::isalnum(c) || c == ':' || c == '/' || c == '.' || c == '-' || c == '_')
            value += get();
        else
            break;
    }

    return Token(TOKEN_STRING, value, startLine, startColumn);
}

Token Lexer::parseString()
{
    std::string value;
    int startLine = _line;
    int startColumn = _column;
    
    while (!isAtEnd() && peek() != '"')
    {
        value += get();
    }
    if (peek() == '"')
    {
       get();
    }
    else
    {
        return Token(TOKEN_UNKNOWN, value, startLine, startColumn);
    }   
    
    return Token(TOKEN_STRING, value, startLine, startColumn);
}

Token Lexer::parseNumber()
{
    std::string value;
    int startLine = _line;
    int startColumn = _column;
    
    while (!isAtEnd() && isdigit(peek()))
    {
        value += get();
    }
    
    return Token(TOKEN_NUMBER, value, startLine, startColumn);
}

Token Lexer::parseSymbol()
{
    int startLine = _line;
    int startColumn = _column;
    char c = get();
    
    switch (c)
    {
        case '{' : 
            return Token(TOKEN_LBRACE, "{", startLine, startColumn);
        case '}' : 
            return Token(TOKEN_RBRACE, "}", startLine, startColumn);
        case ';' : 
            return Token(TOKEN_SEMICOLON, ";", startLine, startColumn);
        case ':' : 
            return Token(TOKEN_COLON, ":", startLine, startColumn);
        default:  
            return Token(TOKEN_UNKNOWN, std::string(1, c), startLine, startColumn);
    }
}

Token Lexer::parsePathLike()
{
    std::string value;
    
    while (!isAtEnd())
    {
        char c = peek();
        if (std::isalnum(c) || c == '/' || c == '.' || c == '-' || c == '_')
        {
            value += get();
        }
        else
            break;
    }
    return Token(TOKEN_STRING, value, _line, _column);
}