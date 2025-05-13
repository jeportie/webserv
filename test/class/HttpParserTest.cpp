/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParserTest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:54:53 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/13 17:28:15 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "../src/class/HttpParser.hpp"
#include <thread>
#include <chrono>

// test/HttpParserTest.cpp
#include <gtest/gtest.h>

// ----------------------
// Tests pour parseMethod
// ----------------------
TEST(ParseMethod, RecognizesStandardMethods) {
    EXPECT_EQ(HttpParser::parseMethod("GET"),    RequestLine::METHOD_GET);
    EXPECT_EQ(HttpParser::parseMethod("POST"),   RequestLine::METHOD_POST);
    EXPECT_EQ(HttpParser::parseMethod("PUT"),    RequestLine::METHOD_PUT);
    EXPECT_EQ(HttpParser::parseMethod("DELETE"), RequestLine::METHOD_DELETE);
}

TEST(ParseMethod, RejectsUnknownOrEmpty) {
    EXPECT_EQ(HttpParser::parseMethod("FETCH"),  RequestLine::METHOD_INVALID);
    EXPECT_EQ(HttpParser::parseMethod(""),       RequestLine::METHOD_INVALID);
    EXPECT_EQ(HttpParser::parseMethod("get"),    RequestLine::METHOD_INVALID); // case-sensitive
}

// ------------------------------
// Tests pour parseRequestLine
// ------------------------------
TEST(ParseRequestLine, ValidRequestLine) {
    RequestLine rl = HttpParser::parseRequestLine("GET /foo/bar?x=1 HTTP/1.1");
    EXPECT_EQ(rl.method,     RequestLine::METHOD_GET);
    EXPECT_EQ(rl.target,     "/foo/bar?x=1");
    EXPECT_EQ(rl.http_major, 1);
    EXPECT_EQ(rl.http_minor, 1);
}

TEST(ParseRequestLine, TooFewTokens) {
    RequestLine rl = HttpParser::parseRequestLine("BADREQUEST");
    EXPECT_EQ(rl.method,     RequestLine::METHOD_INVALID);
    EXPECT_TRUE(rl.target.empty());
    EXPECT_EQ(rl.http_major, 0);
    EXPECT_EQ(rl.http_minor, 0);
}

TEST(ParseRequestLine, BadVersionFormat) {
    RequestLine rl = HttpParser::parseRequestLine("POST / HTTP/X.Y");
    EXPECT_EQ(rl.method,     RequestLine::METHOD_POST);
    EXPECT_EQ(rl.target,     "/");
    EXPECT_EQ(rl.http_major, 0);
    EXPECT_EQ(rl.http_minor, 0);
}

// ------------------------------
// Tests pour parseHeaders
// ------------------------------
TEST(ParseHeaders, ParsesMultipleValidLines) {
    std::string hdrs =
        "Host: example.com\r\n"
        "Content-Type: text/plain\r\n"
        "X-Custom-Header: 42\r\n"
        "\r\n";
    auto m = HttpParser::parseHeaders(hdrs);
    ASSERT_EQ(m.size(), 3u);
    EXPECT_EQ(m["Host"],             "example.com");
    EXPECT_EQ(m["Content-Type"],     "text/plain");
    EXPECT_EQ(m["X-Custom-Header"],  "42");
}

TEST(ParseHeaders, SkipsMalformedLines) {
    std::string hdrs =
        "Good: yes\r\n"
        "NoColonLine\r\n"      // doit être ignorée
        " AlsoBad :\r\n"       // value vide apres trim → ignorée
        "Another: one\r\n"
        "\r\n";
    auto m = HttpParser::parseHeaders(hdrs);
    ASSERT_EQ(m.size(), 2u);
    EXPECT_EQ(m.count("Good"),    1u);
    EXPECT_EQ(m.count("Another"), 1u);
    EXPECT_EQ(m["Good"],          "yes");
    EXPECT_EQ(m["Another"],       "one");
}



TEST(ReadFixedBody, BasicRead) {
    int sv[2];
    // Création de la paire de sockets UNIX
    ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        << "socketpair() a échoué : " << strerror(errno);

    const std::string msg = "HelloWorld";
    // On envoie 10 octets, mais on ne lira que 5
    ssize_t sent = write(sv[0], msg.c_str(), msg.size());
    ASSERT_EQ((ssize_t)msg.size(), sent)
        << "Écriture incomplète sur la socket";

    // Lecture de 5 octets
    std::string result = HttpParser::readFixedBody(sv[1], 5);
    EXPECT_EQ("Hello", result);

    // Nettoyage
    close(sv[0]);
    close(sv[1]);
}

// Test cas où length = 0 : doit renvoyer une chaîne vide sans blocage
TEST(ReadFixedBody, ZeroLength) {
    int sv[2];
    ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        << "socketpair() a échoué : " << strerror(errno);

    std::string result = HttpParser::readFixedBody(sv[1], 0);
    EXPECT_TRUE(result.empty());

    close(sv[0]);
    close(sv[1]);
}

// Test où on demande plus d’octets que ce qui a été envoyé
TEST(ReadFixedBody, RequestMoreThanAvailable) {
    int sv[2];
    ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        << "socketpair() a échoué : " << strerror(errno);

    const std::string msg = "ABC";
    ASSERT_EQ((ssize_t)msg.size(), write(sv[0], msg.c_str(), msg.size()));

    // On demande 10 octets alors que seuls 3 sont dispo
    std::string result = HttpParser::readFixedBody(sv[1], 10);
    EXPECT_EQ("ABC", result);

    close(sv[0]);
    close(sv[1]);
}

