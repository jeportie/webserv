/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidatorTest.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 14:35:21 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/19 13:16:40 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/ConfigValidator.hpp"


#include "../../src/class/ServerConfig.hpp"  


ServerConfig createValidServerConfig() {
    ServerConfig server;
    server.host = "127.0.0.1";
    server.port = 8080;
    server.listenIsSet = true;
    server.root = "/var/www/html";
    server.rootIsSet = true;
    server.client_max_body_size = 1000000;
    server.allowedMethods.push_back("GET");
    server.allowedMethods.push_back("POST");

    RouteConfig route;
    route.path = "/images";
    route.root = "/var/www/images";
    route.autoindex = false;
    server.routes["/images"] = route;

    return server;
}

TEST(ConfigValidatorTest, ValidConfigurationPasses) {
    ServerConfig server = createValidServerConfig();
    std::vector<ServerConfig> servers;
    servers.push_back(server);
    EXPECT_NO_THROW(ConfigValidator().validate(servers));
}


// ✅ IP valide - loopback
TEST(ConfigValidatorTest, ValidIPv4_Localhost) {
    ConfigValidator validator; 
    ServerConfig validConfig = createValidServerConfig();
    EXPECT_NO_THROW(validator.validateServer(validConfig));
}

// ✅ IP valide - général
TEST(ConfigValidatorTest, ValidIPv4_Normal) {
    ConfigValidator validator; 
       ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "192.168.1.42";
    EXPECT_NO_THROW(validator.validateServer(validConfig));
}

// ❌ IP invalide - trop d'octets
TEST(ConfigValidatorTest, InvalidIPv4_TooManyOctets) {
    ConfigValidator validator; 
        ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "192.168.1.1.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - pas assez d'octets
TEST(ConfigValidatorTest, InvalidIPv4_TooFewOctets) {
    ConfigValidator validator; 
       ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "192.168.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - caractères non numériques
TEST(ConfigValidatorTest, InvalidIPv4_NonNumeric) {
    ConfigValidator validator; 
       ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "abc.def.ghi.jkl";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - un octet hors plage (>255)
TEST(ConfigValidatorTest, InvalidIPv4_OutOfRange) {
    ConfigValidator validator; 
       ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "256.100.50.25";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - vide
TEST(ConfigValidatorTest, InvalidIPv4_Empty) {
    ConfigValidator validator; 
        ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - caractères spéciaux
TEST(ConfigValidatorTest, InvalidIPv4_SpecialChars) {
    ConfigValidator validator; 
        ServerConfig validConfig = createValidServerConfig();

    validConfig.host = "127.#$.0.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}


TEST(ConfigValidatorTest, MissingListenDirectiveFails) {
    ServerConfig server = createValidServerConfig();
    server.listenIsSet = false;

    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, InvalidMethodFails) {
    ServerConfig server = createValidServerConfig();
    server.allowedMethods.push_back("PUT");

    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, InvalidHttpStatusCodeFails) {
    ServerConfig server = createValidServerConfig();
    server.error_pages[99] = "/errors/99.html"; // Invalide

    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}


TEST(ConfigValidatorTest, MissingRouteRootAndReturnFails) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/empty"; // Ni root ni returnCode
    server.routes["/empty"] = route;

    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, RouteWithBothRootAndReturnFails) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/conflict";
    route.root = "/var/www";
    route.returnCodes[301] = "/new-location";
    server.routes["/conflict"] = route;

    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}



TEST(ConfigValidatorTest, ValidDefaultFile) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/valid";
    route.root = "/var/www";
    route.defaultFile = "index.html"; // Valide : ne commence pas par / et ne contient pas /
    
    server.routes["/valid"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_NO_THROW(ConfigValidator().validate(servers));
}


TEST(ConfigValidatorTest, DefaultFileStartsWithSlashThrows) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/invalid";
    route.root = "/var/www";
    route.defaultFile = "/index.html"; // Invalide : commence par /
    
    server.routes["/invalid"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, DefaultFileContainsSlashThrows) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/invalid";
    route.root = "/var/www";
    route.defaultFile = "pages/index.html"; // Invalide : contient un /
    
    server.routes["/invalid"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}


TEST(ConfigValidatorTest, EmptyDefaultFileIsAccepted) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/no-default-file";
    route.root = "/var/www";
    route.defaultFile = ""; // Valide : champ vide
    
    server.routes["/no-default-file"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_NO_THROW(ConfigValidator().validate(servers));
}


TEST(ConfigValidatorTest, ValidReturnCodeDirectivePasses) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/redirect";
    route.root = "";
    route.returnCodes[301] = "/moved-permanently";
    server.routes["/redirect"] = route;

    std::vector<ServerConfig> servers(1, server);
    EXPECT_NO_THROW(ConfigValidator().validate(servers));
}

TEST(ConfigValidatorTest, InvalidReturnCodeFails) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/invalid-code";
    route.root = "/var/www";
    route.returnCodes[42] = "/nowhere"; // 42 est invalide

    server.routes["/invalid-code"] = route;
    std::vector<ServerConfig> servers(1, server);

    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, EmptyReturnUrlFails) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/empty-url";
    route.root = "/var/www";
    route.returnCodes[302] = ""; // URL vide : invalide

    server.routes["/empty-url"] = route;
    std::vector<ServerConfig> servers(1, server);

    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}


TEST(ConfigValidatorTest, CgiExecutorWithAbsolutePathIsValid) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/php";
    route.root = "/var/www";
    route.cgiExecutor = std::make_pair(".php", "/usr/bin/php-cgi");
    
    server.routes["/php"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_NO_THROW(ConfigValidator().validate(servers));
}

TEST(ConfigValidatorTest, CgiExecutorWithRelativePathThrows) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/php";
    route.root = "/var/www";
    route.cgiExecutor = std::make_pair(".php", "usr/bin/php-cgi"); // Ne commence pas par '/'
    
    server.routes["/php"] = route;
    std::vector<ServerConfig> servers(1, server);
    
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}

TEST(ConfigValidatorTest, UploadEnabledWithoutStoreFails) {
    ServerConfig server = createValidServerConfig();
    RouteConfig route;
    route.path = "/upload";
    route.root = "/var/www/upload";
    route.uploadEnabled = true; // Pas de uploadStore
    server.routes["/upload"] = route;
    
    std::vector<ServerConfig> servers(1, server);
    EXPECT_THROW(ConfigValidator().validate(servers), std::runtime_error);
}