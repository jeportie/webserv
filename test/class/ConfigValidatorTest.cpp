/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidatorTest.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 14:35:21 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 16:25:12 by fsalomon         ###   ########.fr       */
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

TEST(ConfigValidatorTest, InvalidHttpStatusCodeFails) {
    ServerConfig server = createValidServerConfig();
    server.error_pages[99] = "/errors/99.html"; // Invalide

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


