/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestValidatorTest.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 10:24:00 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/30 18:21:39 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../src/class/Http/RequestValidator.hpp"
#include "../../src/class/Http/HttpRequest.hpp"
#include "../../src/class/ConfigFile/ServerConfig.hpp"
#include "../../src/class/ConfigFile/RouteConfig.hpp"

#include <gtest/gtest.h>


// Helpers
HttpRequest makeRequest(HttpRequest::Method method, const std::string& path, const std::string& body = "") {
    HttpRequest req;
    req.method = method;
    req.path = path;
    req.body = body;
    req.headers["Host"].push_back("localhost");
    return req;
}

ServerConfig makeServerConfig() {
    ServerConfig config;
    config.allowedMethods.push_back("GET");
    config.allowedMethods.push_back("POST");
    config.client_max_body_size = 1000;

    // /api route
    RouteConfig apiRoute;
    apiRoute.allowedMethods.push_back("POST");
    apiRoute.client_max_body_size = 500;
    apiRoute.returnCodes[404] = "/custom_404.html";
    config.routes["/api"] = apiRoute;

    // /api/user route
    RouteConfig userRoute;
    userRoute.allowedMethods.push_back("POST");
    userRoute.client_max_body_size = 500;
    userRoute.returnCodes[404] = "/user_404.html";
    config.routes["/api/user"] = userRoute;

    // /api/user/settings route
    RouteConfig settingsRoute;
    settingsRoute.allowedMethods.push_back("POST");
    settingsRoute.client_max_body_size = 500;
    settingsRoute.returnCodes[404] = "/settings_404.html";
    config.routes["/api/user/settings"] = settingsRoute;

    // Root route
    RouteConfig rootRoute;
    rootRoute.allowedMethods.push_back("POST");
    rootRoute.allowedMethods.push_back("GET");
    rootRoute.client_max_body_size = 500;
    config.routes["/"] = rootRoute;

    return config;
}


// === TESTS ===


TEST(RequestValidatorTest, InvalidMethodThrows405) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_DELETE, "/api");
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    EXPECT_THROW(validator.validate(), HttpException);
}

TEST(RequestValidatorTest, GetMethodNotAllowedThrows405) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/api");  // ❌ GET non autorisé sur /api
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException to be thrown";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 405);  // ✅ Vérifie le bon code
        EXPECT_EQ(std::string(ex.what()), "Method Not Allowed");
    } catch (...) {
        FAIL() << "Expected HttpException, but caught different exception";
    }
}

TEST(RequestValidatorTest, TooLargeBodyThrows413) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_POST, "/api", std::string(600, 'x'));
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 413);
        EXPECT_EQ(std::string(ex.what()), "Payload Too Large");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}


TEST(RequestValidatorTest, GetWithBodyThrows400) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/api", "should_not_be_here");
    ServerConfig config = makeServerConfig();
    
    config.routes["/api"].allowedMethods.push_back("GET");
    
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 400);
        EXPECT_EQ(std::string(ex.what()), "Bad Request: Unexpected body in GET or DELETE request");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}


TEST(RequestValidatorTest, MultipleHostHeadersThrows400) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/");
    req.headers["Host"].push_back("duplicate");
    
    ServerConfig config = makeServerConfig();
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 400);
        EXPECT_EQ(std::string(ex.what()), "Bad Request: Multiple Host header");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}


TEST(RequestValidatorTest, InvalidContentLengthThrows400) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/");
    req.headers["Content-Length"].push_back("abc");
    
    ServerConfig config = makeServerConfig();
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 400);
        EXPECT_EQ(std::string(ex.what()), "Bad Request: Invalid Content-Length");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}

TEST(RequestValidatorTest, ContentLengthAndTransferEncodingThrows400) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/");
    req.headers["Content-Length"].push_back("10");
    req.headers["Transfer-Encoding"].push_back("chunked");
    
    ServerConfig config = makeServerConfig();
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 400);
        EXPECT_EQ(std::string(ex.what()), "Bad Request: Content-Length and Transfer-Encoding are mutually exclusive");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}

TEST(RequestValidatorTest, DuplicateNonWhitelistedHeaderThrows400) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/");
    req.headers["X-Foo"].push_back("bar1");
    req.headers["X-Foo"].push_back("bar2");
    
    ServerConfig config = makeServerConfig();
    RequestValidator validator(req, config);
    
    try {
        validator.validate();
        FAIL() << "Expected HttpException";
    } catch (const HttpException& ex) {
        EXPECT_EQ(ex.status(), 400);
        EXPECT_EQ(std::string(ex.what()), "Bad Request: Duplicate header: X-Foo");
    } catch (...) {
        FAIL() << "Expected HttpException";
    }
}


TEST(RequestValidatorTest, CustomErrorPageReturnedCorrectly) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_POST, "/api");
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    validator.validate();
    
    std::string errorPage = validator.getErrorPage(404);
    EXPECT_EQ(errorPage, "/custom_404.html");
    
    std::string fallback = validator.getErrorPage(500);
    EXPECT_EQ(fallback, "");  // Aucun fallback défini pour 500
}

TEST(RequestValidatorTest, NoCustomErrorPageReturnsEmpty) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/unknown");
    ServerConfig config = makeServerConfig();
    
    //clear les errors pages de /
    config.routes["/"].returnCodes.clear();
    RequestValidator validator(req, config);
    validator.validate();
    
    std::string errorPage = validator.getErrorPage(404);
    EXPECT_EQ(errorPage, "");  // Pas de page d'erreur personnalisée pour 404
}

TEST(RequestValidatorTest, MatchRouteFindsExactPrefix) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_POST, "/api/user/settings");
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());
    
    const RouteConfig* matched = validator.matchedRoute();
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "/api/user/settings");
}


TEST(RequestValidatorTest, MatchRouteFindsLongestPrefix) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_POST, "/api/user/settings/privacy");
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());
    
    const RouteConfig* matched = validator.matchedRoute();
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "/api/user/settings");
}


TEST(RequestValidatorTest, MatchRouteFallsBackToApiPrefix) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_POST, "/api/other");
    ServerConfig config = makeServerConfig();

    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());

    const RouteConfig* matched = validator.matchedRoute();
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "/api");
}

TEST(RequestValidatorTest, MatchRouteReturnsNullIfNoMatch) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/unmatched/path");
    ServerConfig config = makeServerConfig();  // No "/" defined as fallback

    config.routes.clear();  // Clear all routes to simulate no match
    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());

    EXPECT_EQ(validator.matchedRoute(), nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "");
}

TEST(RequestValidatorTest, MatchRouteFallbacksToRoot) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/totally/unknown/path");
    ServerConfig config = makeServerConfig();
    config.routes["/"] = RouteConfig();
    config.routes["/"].allowedMethods.push_back("GET");

    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());

    EXPECT_NE(validator.matchedRoute(), nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "/");
}
TEST(RequestValidatorTest, MatchRouteWithEmptyPath) {
    HttpRequest req = makeRequest(HttpRequest::METHOD_GET, "/coucou");
    ServerConfig config = makeServerConfig();
    
    RequestValidator validator(req, config);
    ASSERT_NO_THROW(validator.validate());
    
    const RouteConfig* matched = validator.matchedRoute();
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(validator.matchedPrefix(), "/");
}