/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerTest.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:49:56 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/13 15:03:47 by fsalomon         ###   ########.fr       */
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

TEST(TestLexer, ErrorToken)
{
    std::string input = "@@@";
    Lexer object(input);
    Token t = object.nextToken();
    EXPECT_EQ(t.type, TOKEN_UNKNOWN);
}