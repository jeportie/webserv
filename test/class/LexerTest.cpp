/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerTest.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:49:56 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/19 10:45:35 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <gtest/gtest.h>
#include "../../src/class/Lexer.hpp"


inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    switch (type) {
        case TOKEN_IDENTIFIER: os << "TOKEN_IDENTIFIER"; break;
        case TOKEN_STRING:     os << "TOKEN_STRING"; break;
        case TOKEN_NUMBER:     os << "TOKEN_NUMBER"; break;
        case TOKEN_COLON:      os << "TOKEN_COLON"; break;
        case TOKEN_SEMICOLON:  os << "TOKEN_SEMICOLON"; break;
        case TOKEN_LBRACE:     os << "TOKEN_LBRACE"; break;
        case TOKEN_RBRACE:     os << "TOKEN_RBRACE"; break;
        case TOKEN_EOF:        os << "TOKEN_EOF"; break;
        case TOKEN_UNKNOWN:    os << "TOKEN_UNKNOWN"; break;
        default:               os << "UNKNOWN_TOKEN_TYPE"; break;
    }
    return os;
}

TEST(TestLexer, parseIdentifier)
{
    std::string input = "server;";
    Lexer object(input);
    

    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value,"server");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t2.value,";");
    
    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_EOF);
}

TEST(TestLexer, parseSymbolParseNumber)
{
    std::string input = "server {\n    listen 8080;\n}";
    Lexer object(input);
    EXPECT_EQ(object.nextToken().type, TOKEN_IDENTIFIER);
    EXPECT_EQ(object.nextToken().type, TOKEN_LBRACE);
    EXPECT_EQ(object.nextToken().type, TOKEN_IDENTIFIER);
    EXPECT_EQ(object.nextToken().type, TOKEN_NUMBER);
    EXPECT_EQ(object.nextToken().type, TOKEN_SEMICOLON);
    EXPECT_EQ(object.nextToken().type, TOKEN_RBRACE);
    EXPECT_EQ(object.nextToken().type, TOKEN_EOF);

}

TEST(TestLexer, parseString)
{
    std::string input = "root /var/www;";
    Lexer object(input);
    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value, "root");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_STRING);
    EXPECT_EQ(t2.value,"/var/www");
    
    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t3.value,";");
    
    Token t4 = object.nextToken();
    EXPECT_EQ(t4.type, TOKEN_EOF);
}

TEST(TestLexer, parsePathLike)
{
    std::string input = "location /images/icons;";
    Lexer object(input);

    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value, "location");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_STRING);
    EXPECT_EQ(t2.value, "/images/icons");

    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t3.value, ";");

    Token t4 = object.nextToken();
    EXPECT_EQ(t4.type, TOKEN_EOF);
}

TEST(TestLexer, parsePathLikeWithDashAndUnderscore)
{
    std::string input = "root /static/js/script-v1_2.js;";
    Lexer object(input);

    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value, "root");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_STRING);
    EXPECT_EQ(t2.value, "/static/js/script-v1_2.js");

    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t3.value, ";");

    Token t4 = object.nextToken();
    EXPECT_EQ(t4.type, TOKEN_EOF);
}

TEST(TestLexer, parsePathLikeWithDotPrefix)
{
    std::string input = "include ./config-site;";
    Lexer object(input);

    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value, "include");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_STRING);
    EXPECT_EQ(t2.value, "./config-site");

    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t3.value, ";");

    Token t4 = object.nextToken();
    EXPECT_EQ(t4.type, TOKEN_EOF);
}

TEST(TestLexer, parsePathLikeStopsAtInvalidCharacter)
{
    std::string input = "upload_store /uploads/images!;";
    Lexer object(input);

    Token t1 = object.nextToken();
    EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
    EXPECT_EQ(t1.value, "upload_store");

    Token t2 = object.nextToken();
    EXPECT_EQ(t2.type, TOKEN_STRING);
    EXPECT_EQ(t2.value, "/uploads/images"); // le "!" ne fait pas partie du chemin

    Token t3 = object.nextToken();
    EXPECT_EQ(t3.type, TOKEN_UNKNOWN); // le "!" n'étant pas reconnu, à adapter selon ton implémentation
    EXPECT_EQ(t3.value, "!");

    Token t4 = object.nextToken();
    EXPECT_EQ(t4.type, TOKEN_SEMICOLON);
    EXPECT_EQ(t4.value, ";");

    Token t5 = object.nextToken();
    EXPECT_EQ(t5.type, TOKEN_EOF);

}


TEST(TestLexer, ErrorToken)
{
    std::string input = "@@@";
    Lexer object(input);
    Token t = object.nextToken();
    EXPECT_EQ(t.type, TOKEN_UNKNOWN);
}