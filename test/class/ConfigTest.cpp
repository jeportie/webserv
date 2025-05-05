/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 19:04:33 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/05 20:11:26 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "../../src/class/Config.hpp"
#include <exception>


TEST(ConfigTest, DefaultConstructor)
{
    Config c;
    EXPECT_EQ(c.getConfigFilePath(), "exconfig.conf");
    EXPECT_FALSE(c.isConfigFileOpen());
}

TEST(ConfigTest, ParseConfigFile)
{
    Config c;
    EXPECT_NO_THROW(c.parseConfigFile("/home/naphy/42/webserv/config_files/exconfig.conf"));
    EXPECT_THROW(c.parseConfigFile("non_existent_file.conf"), std::runtime_error);
    EXPECT_TRUE(c.isConfigFileOpen());
}

// TEST(ConfigTest, ParseLine)
// {
//     Config c;
//     EXPECT_NO_THROW(c.parseLine("server {"));
//     EXPECT_THROW(c.parseLine("unknown_directive"), std::runtime_error);
// }

// TEST(ConfigTest, ParseServerBlock)
// {
//     Config c;
//     EXPECT_NO_THROW(c.parseServerBlock("server { listen 8080; server_name localhost; root /var/www/html; index index.html; }"));
//     EXPECT_THROW(c.parseServerBlock("invalid_server_block"), std::runtime_error);
//     EXPECT_THROW(c.parseServerBlock("server { server { listen 8080; server_name localhost; root /var/www/html; index index.html; } }"), std::runtime_error);
//     // Start to listen std
//     testing::internal::CaptureStdout();
//     EXPECT_NO_THROW(c.parseServerBlock("server { listen 8080; server_name localhost; root /var/www/html; index index.html; }"));
//     std::string out = testing::internal::GetCapturedStdout();
//     EXPECT_EQ(out, "The value of port is : 8080 \nThe value of serverName is : localhost\nThe value of root is : /var/www/html\n The value of index is : index.html\nThe value of errorPage is : 404\n");
// }


