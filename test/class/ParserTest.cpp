/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:09:54 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/19 11:16:45 by fsalomon         ###   ########.fr       */
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

TEST(ParserTest, ParseListenDirectiveWithHostAndPort) {
    std::string input = "listen 127.0.0.1:8080;";
    Lexer lexer(input);
    Parser parser(lexer);

    std::string host;
    int port = 0;

    try {
        parser.parseListenDirective(host, port);
        EXPECT_EQ(host, "127.0.0.1");
        EXPECT_EQ(port, 8080);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseListenDirectiveWithHostAndPort: " << e.what();
    }
}

TEST(ParserTest, ParseListenDirectiveWithPortOnly) {
    std::string input = "listen 8080;";
    Lexer lexer(input);
    Parser parser(lexer);

    std::string host;
    int port = 0;

    try {
        parser.parseListenDirective(host, port);
        EXPECT_EQ(host, "0.0.0.0");  // default host
        EXPECT_EQ(port, 8080);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseListenDirectiveWithPortOnly: " << e.what();
    }
}

TEST(ParserTest, ParseListenDirectiveWithHostOnly) {
    std::string input = "listen localhost;";
    Lexer lexer(input);
    Parser parser(lexer);

    std::string host;
    int port = 0;

    try {
        parser.parseListenDirective(host, port);
        EXPECT_EQ(host, "localhost");
        EXPECT_EQ(port, 80);  // default port
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseListenDirectiveWithHostOnly: " << e.what();
    }
}

TEST(ParserTest, ParseListenDirectiveMissingSemicolon) {
    std::string input = "listen 127.0.0.1:8080";
    Lexer lexer(input);
    Parser parser(lexer);

    std::string host;
    int port = 0;

    try {
        parser.parseListenDirective(host, port);
        FAIL() << "Expected exception due to missing semicolon";
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Expected ';' after listen directive");
    }
}

TEST(ParserTest, ParseListenDirectiveInvalidPort) {
    std::string input = "listen 127.0.0.1:abc;";
    Lexer lexer(input);
    Parser parser(lexer);

    std::string host;
    int port = 0;

    try {
        parser.parseListenDirective(host, port);
        FAIL() << "Expected exception due to invalid port";
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Expected port number after ':' in 'listen' directive");
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

TEST(ParserTest, ParseAllowedMethodsDirective) {
    std::string input = "allowed_methods GET POST;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::vector<std::string> allowedMethods = parser.parseAllowedMethodsDirective();
        EXPECT_EQ(allowedMethods.size(), 2);
        EXPECT_EQ(allowedMethods[0], "GET");
        EXPECT_EQ(allowedMethods[1], "POST");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseAllowedMethodsDirective: " << e.what();
    }
}

TEST(ParserTest, ParseErrorPagesDirective) {
    std::string input = "error_page 404 /errors/404.html 500 /errors/500.html;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<int, std::string> errorPages = parser.parseErrorPagesDirective();
        EXPECT_EQ(errorPages.size(), 2);
        EXPECT_EQ(errorPages[404], "/errors/404.html");
        EXPECT_EQ(errorPages[500], "/errors/500.html");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseErrorPagesDirective: " << e.what();
    }
}

TEST(ParserTest, ParseClientMaxBodySizeDirective) {
    std::string input = "client_max_body_size 1048576;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        size_t clientMaxBodySize = parser.parseClientMaxBodySizeDirective();
        EXPECT_EQ(clientMaxBodySize, 1048576);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseClientMaxBodySizeDirective: " << e.what();
    }
}

TEST(ParserTest, ParseDefaultFileDirective) {
    std::string input = "default_file index.html;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::string defaultFile = parser.parseDefaultFileDirective();
        EXPECT_EQ(defaultFile, "index.html");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseDefaultFileDirective: " << e.what();
    }
}

TEST(ParserTest, ParseReturnDirective) {
    std::string input = "return 301 /moved;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<int, std::string> returnCodes = parser.parseReturnDirective();
        EXPECT_EQ(returnCodes.size(), 1);
        EXPECT_EQ(returnCodes[301], "/moved");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseReturnDirective: " << e.what();
    }

    input = "return 404 /not_found;";
    Lexer lexer2(input);
    Parser parser2(lexer2);

    try {
        std::map<int, std::string> returnCodes = parser2.parseReturnDirective();
        EXPECT_EQ(returnCodes.size(), 1);
        EXPECT_EQ(returnCodes[404], "/not_found");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseReturnDirective (404): " << e.what();
    }
}

TEST(ParserTest, ParseCgiExecutorsDirective) {
    std::string input = "cgi_executor .php /usr/bin/php-cgi;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::pair<std::string, std::string> cgiExecutors = parser.parseCgiExecutorsDirective();
        EXPECT_EQ(cgiExecutors.second, "/usr/bin/php-cgi");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseCgiExecutorsDirective: " << e.what();
    }
}

TEST(ParserTest, ParseUploadEnabledDirective) {
    std::string input = "upload_enable on;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        bool uploadEnabled = parser.parseUploadEnabledDirective();
        EXPECT_TRUE(uploadEnabled);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseUploadEnabledDirective: " << e.what();
    }

    input = "upload_enable off;";
    Lexer lexer2(input);
    Parser parser2(lexer2);

    try {
        bool uploadEnabled = parser2.parseUploadEnabledDirective();
        EXPECT_FALSE(uploadEnabled);
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseUploadEnabledDirective (off): " << e.what();
    }
}

TEST(ParserTest, ParseUploadStoreDirective) {
    std::string input = "upload_store /var/uploads;";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::string uploadStore = parser.parseUploadStoreDirective();
        EXPECT_EQ(uploadStore, "/var/uploads");
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseUploadStoreDirective: " << e.what();
    }
}


TEST(ParserTest, ParseLocationBlocks) {
    std::string input =
        "location /images/ { root /data/images; autoindex on; allowed_methods GET POST; "
        "default_file index.html; return 404 /errors/404.html; "
        "cgi_executor .php /usr/bin/php-cgi; upload_enable on; upload_store /var/uploads; }";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, RouteConfig> locations = parser.parseLocationBlocks();
        ASSERT_EQ(locations.size(), 1);
        ASSERT_TRUE(locations.find("/images/") != locations.end());
        RouteConfig& route = locations["/images/"];
        
        // Vérifications des différents attributs de RouteConfig
        EXPECT_EQ(route.root, "/data/images");
        EXPECT_TRUE(route.autoindex);
        EXPECT_EQ(route.allowedMethods.size(), 2);
        EXPECT_EQ(route.allowedMethods[0], "GET");
        EXPECT_EQ(route.allowedMethods[1], "POST");
        EXPECT_EQ(route.defaultFile, "index.html");
        EXPECT_EQ(route.returnCodes.size(), 1);
        EXPECT_EQ(route.returnCodes[404], "/errors/404.html");
        EXPECT_EQ(route.cgiExecutor.second, "/usr/bin/php-cgi");
        EXPECT_TRUE(route.uploadEnabled);
        EXPECT_EQ(route.uploadStore, "/var/uploads");
        
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseLocationBlocks: " << e.what();
    }
}

TEST(ParserTest, ParseServerBlock) {
    std::string input =
        "server { listen 8080; server_name example.com; root /var/www;  "
        "allowed_methods GET POST; error_page 404 /errors/404.html; "
        "location /img/ { root /data/img; autoindex off; allowed_methods GET; "
        "return 301 /moved; cgi_executor .php /usr/bin/php-cgi; "
        "upload_enable off; upload_store /tmp/uploads; } }";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        ServerConfig config = parser.parseServerBlock();
        
        // Vérifications des attributs de ServerConfig
        EXPECT_EQ(config.port, 8080);
        EXPECT_EQ(config.root, "/var/www");
        EXPECT_EQ(config.serverNames.size(), 1);
        EXPECT_EQ(config.serverNames[0], "example.com");
        EXPECT_EQ(config.allowedMethods.size(), 2);
        EXPECT_EQ(config.allowedMethods[0], "GET");
        EXPECT_EQ(config.allowedMethods[1], "POST");
        EXPECT_EQ(config.error_pages.size(), 1);
        EXPECT_EQ(config.error_pages[404], "/errors/404.html");

        // Vérification des routes
        ASSERT_EQ(config.routes.size(), 1);
        RouteConfig& route = config.routes["/img/"];
        
        EXPECT_EQ(route.path, "/img/");
        EXPECT_EQ(route.root, "/data/img");
        EXPECT_FALSE(route.autoindex);
        EXPECT_EQ(route.allowedMethods.size(), 1);
        EXPECT_EQ(route.allowedMethods[0], "GET");
        EXPECT_EQ(route.returnCodes.size(), 1);
        EXPECT_EQ(route.returnCodes[301], "/moved");
        EXPECT_EQ(route.cgiExecutor.second, "/usr/bin/php-cgi");
        EXPECT_FALSE(route.uploadEnabled);
        EXPECT_EQ(route.uploadStore, "/tmp/uploads");

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseServerBlock: " << e.what();
    }
}


TEST(ParserTest, ParseConfigFileSingleServer) {
    std::string input = 
        "server { "
        "listen 127.0.0.1:8080; "
        "server_name example.com; "
        "}";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, std::vector<ServerConfig> > result = parser.parseConfigFile();

        EXPECT_EQ(result.size(), 1);

        std::map<std::string, std::vector<ServerConfig> >::iterator it = result.find("127.0.0.1:8080");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 1);

        ServerConfig& config = it->second[0];
        EXPECT_EQ(config.port, 8080);
        EXPECT_EQ(config.host, "127.0.0.1");
        ASSERT_EQ(config.serverNames.size(), 1);
        EXPECT_EQ(config.serverNames[0], "example.com");

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseConfigFileSingleServer: " << e.what();
    }
}

TEST(ParserTest, ParseConfigFileMultipleServersSameHostPort) {
    std::string input = 
        "server { listen 0.0.0.0:80; server_name site1.com; } "
        "server { listen 0.0.0.0:80; server_name site2.com; }";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, std::vector<ServerConfig> > result = parser.parseConfigFile();

        EXPECT_EQ(result.size(), 1);

        std::map<std::string, std::vector<ServerConfig> >::iterator it = result.find("0.0.0.0:80");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 2);

        EXPECT_EQ(it->second[0].serverNames[0], "site1.com");
        EXPECT_EQ(it->second[1].serverNames[0], "site2.com");

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseConfigFileMultipleServersSameHostPort: " << e.what();
    }
}

TEST(ParserTest, ParseConfigFileNoServerBlockThrows) {
    std::string input = "listen 80;";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        parser.parseConfigFile();
        FAIL() << "Expected std::runtime_error not thrown";
    } catch (const std::runtime_error& e) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Expected std::runtime_error but caught different exception";
    }
}

TEST(ParserTest, ParseConfigFileServerWithNoNameUsesDefault) {
    std::string input = 
        "server { listen 192.168.0.1:9090; }";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, std::vector<ServerConfig> > result = parser.parseConfigFile();

        EXPECT_EQ(result.size(), 1);

        std::map<std::string, std::vector<ServerConfig> >::iterator it = result.find("192.168.0.1:9090");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 1);

        ServerConfig& config = it->second[0];
        ASSERT_EQ(config.serverNames.size(), 1);
        EXPECT_EQ(config.serverNames[0], "default");

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseConfigFileServerWithNoNameUsesDefault: " << e.what();
    }
}

TEST(ParserTest, ParseConfigFileMultipleServersDifferentHostPortsAndNames) {
    std::string input =
        "server { listen 192.168.1.10:8080; server_name siteA.com; } "
        "server { listen 192.168.1.11:8081; server_name siteB.com; } "
        "server { listen 192.168.1.12:8081; server_name siteA.com; }";  // même nom que siteA.com mais host:port différent

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, std::vector<ServerConfig> > result = parser.parseConfigFile();

        // Il doit y avoir 3 clés différentes car host:port tous différents
        EXPECT_EQ(result.size(), 3);

        // Vérifie chaque serveur individuellement
        std::map<std::string, std::vector<ServerConfig> >::iterator it;

        it = result.find("192.168.1.10:8080");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 1);
        EXPECT_EQ(it->second[0].serverNames[0], "siteA.com");

        it = result.find("192.168.1.11:8081");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 1);
        EXPECT_EQ(it->second[0].serverNames[0], "siteB.com");

        it = result.find("192.168.1.12:8081");
        ASSERT_NE(it, result.end());
        EXPECT_EQ(it->second.size(), 1);
        EXPECT_EQ(it->second[0].serverNames[0], "siteA.com");  // même nom que premier serveur mais host:port différent

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseConfigFileMultipleServersDifferentHostPortsAndNames: " << e.what();
    }
}

TEST(ParserTest, ParseConfigFileDifferentHostPortsSameServerName) {
    std::string input =
        "server { listen 10.0.0.1:80; server_name sameName.com; } "
        "server { listen 10.0.0.2:8080; server_name sameName.com; } "
        "server { listen 10.0.0.3:443; server_name sameName.com; }";

    Lexer lexer(input);
    Parser parser(lexer);

    try {
        std::map<std::string, std::vector<ServerConfig> > result = parser.parseConfigFile();

        // Trois host:port différents, donc 3 clés dans la map
        EXPECT_EQ(result.size(), 3);

        // Vérifier que chaque serveur a le même server_name "sameName.com"
        for (std::map<std::string, std::vector<ServerConfig> >::iterator it = result.begin();
             it != result.end(); ++it) {
            EXPECT_EQ(it->second.size(), 1);
            EXPECT_EQ(it->second[0].serverNames.size(), 1);
            EXPECT_EQ(it->second[0].serverNames[0], "sameName.com");
        }

    } catch (const std::exception& e) {
        FAIL() << "Exception thrown in ParseConfigFileDifferentHostPortsSameServerName: " << e.what();
    }
}

