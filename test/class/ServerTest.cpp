/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:06:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/13 14:23:46 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <gtest/gtest.h>
// #include <arpa/inet.h>
// #include "../../src/class/Server.hpp"
// #include <thread>
// #include <chrono>
//
// TEST(ServerTest, DefaultConstructor)
// {
//     Server o;
//
// 	EXPECT_EQ(o.getServerSocket(), -1);
// 	EXPECT_EQ(o.getClientSocket(), -1);
// }
//
//
// TEST(ServerTest, InitConnect)
// {
//     Server o;
//
//
// 	EXPECT_EQ(o.safeBind(), true);
// 	EXPECT_EQ(o.safeListen(), true);
	// EXPECT_EQ(o.safeAccept(), true);
//
//     ASSERT_NE(o.getServerSocket(), -1);
    // ASSERT_NE(o.getClientSocket(), -1);
//
	// Start to listen std out
    // testing::internal::CaptureStdout();
    // o.safeAccept();
    // std::string out = testing::internal::GetCapturedStdout();
    // EXPECT_EQ(out, "Connection accepted!\n");
// }
//
//
// TEST(ServerTest, AcceptWithMockTelnetClient)
// {
//     try
//     {
//         /* code */
//
//         Server srv;
//         ASSERT_TRUE(srv.safeBind());
//         ASSERT_TRUE(srv.safeListen());
//         int port = 8666; 
//
//         std::thread t_client([port](){
//             std::this_thread::sleep_for(std::chrono::milliseconds(50));
//
//             int sock = ::socket(AF_INET, SOCK_STREAM, 0);
//             ASSERT_GE(sock, 0);  // fatal if socket() failed
//
//             sockaddr_in addr{};
//             addr.sin_family = AF_INET;
//             addr.sin_port   = htons(port);
//             inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
//
//             ASSERT_EQ(::connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);
//
//             const char* hello = "hello\r\n";
//             ::send(sock, hello, strlen(hello), 0);
//
//             ::close(sock);
//         });
//         EXPECT_TRUE(srv.safeAccept());
//         t_client.join();
//     }
//     catch(...)
//     {
//         std::cerr << "Failure" << '\n';
//     }
// }



// TEST(ServerTest, Communication)
// {
//     Server o;

	// Start to listen std out
    // testing::internal::CaptureStdout();
    // o.safeAccept();
    // std::string out = testing::internal::GetCapturedStdout();
    // EXPECT_EQ(out, "Connection accepted!\n");
// }
