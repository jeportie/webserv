/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:07:10 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/13 12:28:33 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Lexer.hpp"


Lexer::Lexer(){ };

Lexer::Lexer(const Lexer& src){ *this = src;}

Lexer& Lexer::operator=(const Lexer& rhs) { 
    if (this != &rhs) {
        _content = rhs._content;
        _pos = rhs._pos;
        _line = rhs._line;
        _column = rhs._column;
 }
}

Lexer::~Lexer(void) {}


Lexer::Lexer(const std::string &content)
{
    _content = content;
    _pos = 0;
    _line = 1;
    _column = 1;
}

Token Lexer::nextToken(){}

char Lexer::peek() const
{
    if (_pos >= _content.size())
        return (0);
    return (_content[_pos]);
}

char Lexer::get()
{
    if (_pos >= _content.size())
        return (0);
    char c = _content[_pos++];
    
        if (c == '\n')
        {
            _line ++;
            _column = 1;
        }
        else
        {
            _column ++;
        }
    return (c);
 
}
bool Lexer::isAtEnd() const {
    return _pos >= _content.size();
}    int startLine = _line;
    int startColumn = _column;

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
    int startLine = _line;
    int startColumn = _column;

    std::string value;

    while (!isAtEnd() && (std::isalnum(peek()) || isValidIdentifierSymbol(peek())))
    {
        value += get();
    }
    return Token(TOKEN_IDENTIFIER, value, startLine, startColumn);
}


Token Lexer::parseString()
{
    int startLine = _line;
    int startColumn = _column;
    
    std::string value;
    while (!isAtEnd() && peek() != '"')
    {
        value += get();
    }
    if (peek() == '"')
    {
       get();
    }    
    
    return Token(TOKEN_STRING, value, startLine, startColumn);
}

Token Lexer::parseNumber()
{
        int startLine = _line;
    int startColumn = _column;
    
    std::string value;
    while (!isAtEnd() && isdigit(peek()))
    {
        value += get();
    }
    
    return Token(TOKEN_NUMBER, value, startLine, startColumn);
}

Token Lexer::ParseSymbol()
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
        case '{' : 
            return Token(TOKEN_LBRACE, "{", startLine, startColumn);
        case ';' : 
            return Token(TOKEN_SEMICOLON, ";", startLine, startColumn);
        case ':' : 
            return Token(TOKEN_COLON, ":", startLine, startColumn);
        default:  
            return Token(TOKEN_UNKNOWN, std::string(1, c), startLine, startColumn);
    }
}

Token Lexer::nextToken()
{
    while (!isAtEnd())
    {
        skipWhitespace();
        skipComment();
        skipWhitespace();

        if (isAtEnd())
            break;
        

        char c = peek();
        if (std::isalpha(c))
        {
            return parseIdentifier();
        }
        else if (std::isdigit(c))
        {
            return parseNumber();
        }
        else if (c == '"')
        {
            return parseString();
        }
        else
        {
            return parseSymbol();
        }
    }
    
    return Token(TOKEN_COLON, "", _line, _column);
}
