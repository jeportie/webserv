/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParserTest.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:54:53 by anastruc          #+#    #+#             */
/*   Updated: 2025/05/30 10:38:48 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "../../src/class/Http/HttpParser.hpp"
#define private public
#define protected public
#undef private
#undef protected
#include "../../src/class/Http/HttpException.hpp"


// test/HttpParserTest.cpp

// ----------------------
// Tests pour parseMethod
// ----------------------
// test/HttpParserTest.cpp



// ----------------------
// Tests pour parseMethod
// ----------------------
TEST(ParseMethod, RecognizesStandardMethods) {
    EXPECT_EQ(HttpParser::parseMethod("GET"),    HttpRequest::METHOD_GET);
    EXPECT_EQ(HttpParser::parseMethod("POST"),   HttpRequest::METHOD_POST);
    EXPECT_EQ(HttpParser::parseMethod("DELETE"), HttpRequest::METHOD_DELETE);
}

TEST(ParseMethod, RejectsUnknownOrEmpty) {
    EXPECT_EQ(HttpParser::parseMethod("FETCH"),  HttpRequest::METHOD_INVALID);
    EXPECT_EQ(HttpParser::parseMethod(""),       HttpRequest::METHOD_INVALID);
    EXPECT_EQ(HttpParser::parseMethod("get"),    HttpRequest::METHOD_INVALID); // case-sensitive
}

// ------------------------------
// Tests pour parseRequestLine
// ------------------------------
TEST(ParseRequestLine, ValidRequestLine) {
    EXPECT_NO_THROW({
        RequestLine rl = HttpParser::parseRequestLine("GET /foo/bar?x=1 HTTP/1.1");
        EXPECT_EQ(rl.method,     HttpRequest::METHOD_GET);
        EXPECT_EQ(rl.target,     "/foo/bar?x=1");
        EXPECT_EQ(rl.http_major, 1);
        EXPECT_EQ(rl.http_minor, 1);
    });
}

TEST(ParseRequestLine, TooFewTokens) {
    try {
        HttpParser::parseRequestLine("BADREQUEST");
        FAIL() << "Expected HttpException";
    }
    catch (const HttpException& e) {
        EXPECT_EQ(e.status(), 400);
    }
    catch (...) {
        FAIL() << "Expected HttpException";
    }
}

TEST(ParseRequestLine, BadVersionFormat) {
    try {
        HttpParser::parseRequestLine("POST / HTTP/X.Y");
        FAIL() << "Expected HttpException";
    }
    catch (const HttpException& e) {
        EXPECT_EQ(e.status(), 505);
    }
    catch (...) {
        FAIL() << "Expected HttpException";
    }
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
    EXPECT_EQ(m["Host"][0],            "example.com");
    EXPECT_EQ(m["Content-Type"][0],    "text/plain");
    EXPECT_EQ(m["X-Custom-Header"][0], "42");
}

TEST(ParseHeaders, SkipsMalformedLines) {
    // Maintenant, un header sans « : » ou valeur vide est considéré comme BAD REQUEST
    std::string hdrs =
        "Good: yes\r\n"
        "NoColonLine\r\n"      // pas de « : »
        "AlsoBad:   \r\n"      // valeur vide après trim
        "Another: one\r\n"
        "\r\n";
    try {
        HttpParser::parseHeaders(hdrs);
        FAIL() << "Expected HttpException";
    }
    catch (const HttpException& e) {
        EXPECT_EQ(e.status(), 400);
    }
    catch (...) {
        FAIL() << "Expected HttpException";
    }
}



// TEST(ReadFixedBody, BasicRead) {
//     int sv[2];
//     // Création de la paire de sockets UNIX
//     ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
//         << "socketpair() a échoué : " << strerror(errno);

//     const std::string msg = "HelloWorld";
//     // On envoie 10 octets, mais on ne lira que 5
//     ssize_t sent = write(sv[0], msg.c_str(), msg.size());
//     ASSERT_EQ((ssize_t)msg.size(), sent)
//         << "Écriture incomplète sur la socket";

//     // Lecture de 5 octets
//     std::string result = HttpParser::readFixedBody(sv[1], 5);
//     EXPECT_EQ("Hello", result);

//     // Nettoyage
//     close(sv[0]);
//     close(sv[1]);
// }

// // Test cas où length = 0 : doit renvoyer une chaîne vide sans blocage
// TEST(ReadFixedBody, ZeroLength) {
//     int sv[2];
//     ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
//         << "socketpair() a échoué : " << strerror(errno);

//     std::string result = HttpParser::readFixedBody(sv[1], 0);
//     EXPECT_TRUE(result.empty());

//     close(sv[0]);
//     close(sv[1]);
// }

// // Test où on demande plus d’octets que ce qui a été envoyé
// TEST(ReadFixedBody, RequestMoreThanAvailable) {
//     int sv[2];
//     ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
//         << "socketpair() a échoué : " << strerror(errno);

//     const std::string msg = "ABC";
//     ASSERT_EQ((ssize_t)msg.size(), write(sv[0], msg.c_str(), msg.size()));
//     close(sv[0]);

//     // On demande 10 octets alors que seuls 3 sont dispo
//     std::string result = HttpParser::readFixedBody(sv[1], 10);
//     EXPECT_EQ("ABC", result);

//     close(sv[1]);
// }


