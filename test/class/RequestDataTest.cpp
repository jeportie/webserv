/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestDataTest.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 18:24:17 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/31 11:48:01 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <gtest/gtest.h>
#define private public
#define protected public
#undef private
#undef protected


#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include "../../src/class/Sockets/Socket.hpp"
#include "../../src/class/Http/RequestLine.hpp"
#include "../../src/class/Http/HttpException.hpp"
#include "../../src/class/Http/RequestData.hpp"
#include "../../src/class/ConfigFile/ServerConfig.hpp"
#include "../../include/webserv.h"

class TestRequestData : public RequestData {
public:
    virtual int getPortFromFd(int fd) {
        // Dans le test, retourne toujours 8080.
        (void)fd;
        return 8080;
    }

    virtual std::string findHostInHeaders() {
        return "localhost";
    }
    
};

TEST(RequestDataTest, InitServerConfig_StoresCorrectConfig) {
    ServerConfig config;
    config.port = 8080;
    config.serverNames.push_back("localhost");

    IVSCMAP configMap;
    configMap[8080].push_back(config);

    TestRequestData req;
    req.setListeningSocketFd(42);  // n’importe quel fd, car getPortFromFd est mocké

    try {
        req.initServerConfig(configMap);
    } catch (const std::exception& e) {
        FAIL() << "initServerConfig threw exception: " << e.what();
    }

    ServerConfig stored = req.getServerConfig();

    EXPECT_EQ(stored.port, 8080);
    EXPECT_EQ(stored.serverNames.size(), 1);
    EXPECT_EQ(stored.serverNames[0], "localhost");
}

TEST(RequestDataTest, InitServerConfig_WithMultipleConfigs) {
    ServerConfig config1;
    config1.port = 8080;
    config1.serverNames.push_back("localhost");
    config1.serverNames.push_back("example.com");

    ServerConfig config2;
    config2.port = 8080;
    config2.serverNames.push_back("test.com");

    ServerConfig config3;
    config3.port = 9090;
    config3.serverNames.push_back("anotherhost");

    IVSCMAP configMap;
    configMap[8080].push_back(config1);
    configMap[8080].push_back(config2);
    configMap[9090].push_back(config3);

    RequestData req;

    // Test cas host "localhost" et port 8080
    try {
        req.initServerConfig(configMap, 8080, "localhost");
    } catch (const std::exception& e) {
        FAIL() << "initServerConfig threw exception: " << e.what();
    }
    ServerConfig stored = req.getServerConfig();
    EXPECT_EQ(stored.port, 8080);
    EXPECT_EQ(stored.serverNames.size(), 2);
    EXPECT_EQ(stored.serverNames[0], "localhost");

    // Test cas host "test.com" et port 8080
    try {
        req.initServerConfig(configMap, 8080, "test.com");
    } catch (const std::exception& e) {
        FAIL() << "initServerConfig threw exception: " << e.what();
    }
    stored = req.getServerConfig();
    EXPECT_EQ(stored.port, 8080);
    EXPECT_EQ(stored.serverNames.size(), 1);
    EXPECT_EQ(stored.serverNames[0], "test.com");

    // Test cas port 9090 et host "anotherhost"
    try {
        req.initServerConfig(configMap, 9090, "anotherhost");
    } catch (const std::exception& e) {
        FAIL() << "initServerConfig threw exception: " << e.what();
    }
    stored = req.getServerConfig();
    EXPECT_EQ(stored.port, 9090);
    EXPECT_EQ(stored.serverNames.size(), 1);
    EXPECT_EQ(stored.serverNames[0], "anotherhost");

    // Test cas port 8080 et host vide -> doit retourner la première config de 8080
    try {
        req.initServerConfig(configMap, 8080, "");
    } catch (const std::exception& e) {
        FAIL() << "initServerConfig threw exception: " << e.what();
    }
    stored = req.getServerConfig();
    EXPECT_EQ(stored.port, 8080);
    EXPECT_EQ(stored.serverNames.size(), 2);
    EXPECT_EQ(stored.serverNames[0], "localhost");

    EXPECT_THROW(req.initServerConfig(configMap, 8080, "1"), HttpException);
    
}