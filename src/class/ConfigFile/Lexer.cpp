/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:07:10 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 09:34:24 by fsalomon         ###   ########.fr       */
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
 return *this;
}

Lexer::~Lexer(void) {}

Lexer::Lexer(const std::string &content)
{
    _content = content;
    _pos = 0;
    _line = 1;
    _column = 1;
}

char Lexer::peek() const
{
    if (_pos >= _content.size())
        return (0);
    return (_content[_pos]);
}

char Lexer::get()
{
    char c;
    
    if (_pos >= _content.size())
        return (0);
    c = _content[_pos++];
    
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

Token Lexer::nextToken()
{
    char c;
    
    while (!isAtEnd())
    {
        while (true)
        {
            skipWhitespace();
            skipComment();

            if (!std::isspace(peek()) && peek() != '#')
                break;
        }
        if (isAtEnd())
            break;
        
        c = peek();
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
        else if (c == '/' || c == '.' || c == '-') 
        {
            return parsePathLike();
        }
        else
        {
            return parseSymbol();
        }
    }
    
    return Token(TOKEN_EOF, "", _line, _column);
}
