/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidatorTest.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 14:35:21 by fsalomon          #+#    #+#             */
/*   Updated: 2025/05/16 15:22:13 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/ConfigValidator.hpp"


#include "../../src/class/ServerConfig.hpp"    // struct ServerConfig



// ✅ IP valide - loopback
TEST(ConfigValidatorTest, ValidIPv4_Localhost) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "127.0.0.1";
    EXPECT_NO_THROW(validator.validateServer(validConfig));
}

// ✅ IP valide - général
TEST(ConfigValidatorTest, ValidIPv4_Normal) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "192.168.1.42";
    EXPECT_NO_THROW(validator.validateServer(validConfig));
}

// ❌ IP invalide - trop d'octets
TEST(ConfigValidatorTest, InvalidIPv4_TooManyOctets) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "192.168.1.1.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - pas assez d'octets
TEST(ConfigValidatorTest, InvalidIPv4_TooFewOctets) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "192.168.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - caractères non numériques
TEST(ConfigValidatorTest, InvalidIPv4_NonNumeric) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "abc.def.ghi.jkl";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - un octet hors plage (>255)
TEST(ConfigValidatorTest, InvalidIPv4_OutOfRange) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "256.100.50.25";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - vide
TEST(ConfigValidatorTest, InvalidIPv4_Empty) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}

// ❌ IP invalide - caractères spéciaux
TEST(ConfigValidatorTest, InvalidIPv4_SpecialChars) {
    ConfigValidator validator; 
    ServerConfig validConfig;
    
        validConfig.listenIsSet = true;
        validConfig.rootIsSet = true;
        validConfig.port = 8080;
        validConfig.host = "127.0.0.1";
        validConfig.root = "/var/www/html";
        validConfig.client_max_body_size = 1000000;
        validConfig.allowedMethods.push_back("GET");
    validConfig.host = "127.#$.0.1";
    EXPECT_THROW(validator.validateServer(validConfig), std::runtime_error);
}
