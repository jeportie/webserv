/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:09:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/13 17:10:37 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <gtest/gtest.h>
#include "../../src/class/Parser.hpp"


TEST(ParserTest, AdvanceAndCurrent) {
    std::string input = "listen 8080;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        Token t1 = parser.current();
        EXPECT_EQ(t1.type, TOKEN_IDENTIFIER);
        EXPECT_EQ(t1.value, "listen");

        parser.advance();
        Token t2 = parser.current();
        EXPECT_EQ(t2.type, TOKEN_NUMBER);
        EXPECT_EQ(t2.value, "8080");

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in AdvanceAndCurrent: " << e.what();
    }
}

TEST(ParserTest, ParseListenDirective) {
    std::string input = "listen 8080;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        int port = parser.parseListenDirective();
        EXPECT_EQ(port, 8080);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseListenDirective: " << e.what();
    }
}

TEST(ParserTest, ParseServerNameDirective) {
    std::string input = "server_name example.com www.example.com;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::vector<std::string> names = parser.parseServerNameDirective();
        ASSERT_EQ(names.size(), 2);
        EXPECT_EQ(names[0], "example.com");
        EXPECT_EQ(names[1], "www.example.com");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseServerNameDirective: " << e.what();
    }
}

TEST(ParserTest, ParseRootDirective) {
    std::string input = "root /var/www/html;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::string root = parser.parseRootDirective();
        EXPECT_EQ(root, "/var/www/html");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseRootDirective: " << e.what();
    }
}

TEST(ParserTest, ParseAutoindexDirective) {
    std::string input = "autoindex on;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        bool autoindex = parser.parseAutoindexDirective();
        EXPECT_TRUE(autoindex);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseAutoindexDirective: " << e.what();
    }

    input = "autoindex off;";
    Lexer lexer2(input);
    Parser parser2(lexer2);

    try {
        bool autoindex = parser2.parseAutoindexDirective();
        EXPECT_FALSE(autoindex);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseAutoindexDirective (off): " << e.what();
    }
}

TEST(ParserTest, ParseLocationBlocks) {
    std::string input =
        "location /images/ { root /data/images; autoindex on; }";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, RouteConfig> locations = parser.parseLocationBlocks();
        ASSERT_EQ(locations.size(), 1);
        ASSERT_TRUE(locations.find("/images/") != locations.end());
        EXPECT_EQ(locations["/images/"].root, "/data/images");
        EXPECT_TRUE(locations["/images/"].autoindex);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseLocationBlocks: " << e.what();
    }
}

TEST(ParserTest, ParseServerBlock) {
    std::string input =
        "server { listen 8080; server_name example.com; root /var/www; autoindex on; "
        "location /img/ { root /data/img; autoindex off; } }";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        ServerConfig config = parser.parseServerBlock();
        EXPECT_EQ(config.port, 8080);
        EXPECT_EQ(config.root, "/var/www");
        EXPECT_TRUE(config.autoindex);
        EXPECT_EQ(config.serverNames.size(), 1);
        EXPECT_EQ(config.serverNames[0], "example.com");
        ASSERT_EQ(config.routes.size(), 1);
        EXPECT_EQ(config.routes["/img/"].root, "/data/img");
        EXPECT_FALSE(config.routes["/img/"].autoindex);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseServerBlock: " << e.what();
    }
}

