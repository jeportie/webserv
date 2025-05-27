/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestDataTest.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 18:24:17 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/27 18:31:06 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <gtest/gtest.h>
#include "../../src/class/Sockets/RequestData.hpp"
#define private public
#define protected public
#undef private
#undef protected


class MockRequestData : public RequestData {
public:
    MockRequestData(int fd) { _listeningSocketFd = fd; }

    std::string findHostInHeaders() const {
        return _hostOverride;
    }

    void setHost(std::string host) {
        _hostOverride = host;
    }

    const ServerConfig& getServerConfig() const {
        return _serverConfig;
    }

private:
    std::string _hostOverride;
};


TEST(RequestDataTest, InitServerConfig_FindsCorrectConfig)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sockfd, -1);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(4242);
    ASSERT_EQ(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)), 0);

    ServerConfig config;
    config.serverNames.push_back("myhost");

    IVSCMAP configMap;
    configMap[4242].push_back(config);

    MockRequestData req(sockfd);
    req.setHost("myhost");

    EXPECT_NO_THROW(req.initServerConfig(configMap));
    EXPECT_EQ(req.getServerConfig().serverNames[0], "myhost");

    close(sockfd);
}


TEST(RequestDataTest, FindMyConfig_WithMatchingPortAndHost)
{
    ServerConfig config1;
    config1.serverNames.push_back("localhost");

    ServerConfig config2;
    config2.serverNames.push_back("example.com");

    IVSCMAP configMap;
    configMap[8080].push_back(config1);
    configMap[8080].push_back(config2);

    ServerConfig result = RequestData::findMyConfig(8080, "example.com", configMap);
    EXPECT_EQ(result.serverNames[0], "example.com");
}

TEST(RequestDataTest, FindMyConfig_WithEmptyHost_ReturnsFirstConfig)
{
    ServerConfig config;
    config.serverNames.push_back("localhost");

    IVSCMAP configMap;
    configMap[8080].push_back(config);

    ServerConfig result = RequestData::findMyConfig(8080, "", configMap);
    EXPECT_EQ(result.serverNames[0], "localhost");
}

TEST(RequestDataTest, FindMyConfig_NoMatchingPort_ThrowsException)
{
    IVSCMAP configMap;
    EXPECT_THROW(RequestData::findMyConfig(9999, "localhost", configMap), HttpException);
}

TEST(RequestDataTest, FindMyConfig_NoMatchingHost_ThrowsException)
{
    ServerConfig config;
    config.serverNames.push_back("localhost");

    IVSCMAP configMap;
    configMap[8080].push_back(config);

    EXPECT_THROW(RequestData::findMyConfig(8080, "unknown.com", configMap), HttpException);
}
