/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 17:07:17 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 12:23:01 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

#include <string>
#include <cctype>

enum TokenType {
    TOKEN_IDENTIFIER, //pour les mots comme server listen root
    TOKEN_STRING, // pour les chemins , les noms (/var/www, localhost)
    TOKEN_NUMBER, // pour des chiffres seules
    TOKEN_COLON, // :
    TOKEN_SEMICOLON, // ;
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // }
    TOKEN_EOF, // fin du fichier
    TOKEN_UNKNOWN // en cas derreur ou char inattendue
};

struct Token {
    TokenType type; // type du token 
    std::string value; // contenu exacte lu
    int line; // ligne du fichier ou le token commence
    int column; // colonne du debut du token
    Token() : type(TOKEN_UNKNOWN), value(""), line(0), column(0) {}

    Token(TokenType t, const std::string& val, int l, int c)
        : type(t), value(val), line(l), column(c) {}
};

class Lexer {
    public:
    Lexer(const std::string &content);
    Token nextToken(); // retourne le porchain token
    ~Lexer();                           
    
    private :
    Lexer();
    Lexer(const Lexer& other);                         
    Lexer& operator=(const Lexer& other);          
    std::string _content; // contenu brut du fichier
    size_t _pos; // position actuelle dans le texte
    int _line; //ligne actuelle
    int _column; //colonne actuelle

    char peek() const; // regarde le prochain char sans avancer 
    char get(); // avance et retourne le prochin char 
    bool isAtEnd() const;
    bool isValidIdentifierSymbol(char c) const;
    bool isNextTokenUrl() const;
    void skipWhitespace(); // Ignore les espaces et les sauts de ligne
    void skipComment(); // Ignore les lignes de commentaire
    Token parseIdentifier();
    Token parseString();
    Token parseNumber();
    Token parseSymbol();
    Token parsePathLike();
    Token parseUrl();

};

#endif 
