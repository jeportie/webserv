/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManagerTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 01:28:02 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/08 01:30:23 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "../../src/class/SocketManager.hpp"
#include <thread>

TEST(SocketManagerTest, SocketCreation)
{
    SocketManager sm;
    EXPECT_TRUE(sm.socketCreate());
    EXPECT_NE(sm.getServerSocket(), -1);
}

TEST(SocketManagerTest, Binding)
{
    SocketManager sm;
    EXPECT_TRUE(sm.socketCreate());
    EXPECT_TRUE(sm.safeBind(PORT, "127.0.0.1"));
}

TEST(SocketManagerTest, Listening)
{
    SocketManager sm;
    EXPECT_TRUE(sm.socketCreate());
    EXPECT_TRUE(sm.safeBind(PORT, "127.0.0.1"));
    EXPECT_TRUE(sm.safeListen(10));
}

TEST(SocketManagerTest, AcceptConnection)
{
    SocketManager sm;
    EXPECT_TRUE(sm.socketCreate());
    EXPECT_TRUE(sm.safeBind(PORT, "127.0.0.1"));
    EXPECT_TRUE(sm.safeListen(10));

    // Simulate a client connection in a separate thread
    std::thread clientThread(
        []()
        {
            int         sock = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(PORT);
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
            connect(sock, (sockaddr*) &addr, sizeof(addr));
            close(sock);
        });

    EXPECT_NO_THROW(sm.safeAccept());
    clientThread.join();
}

TEST(SocketManagerTest, NonBlockingMode)
{
    SocketManager sm;
    EXPECT_TRUE(sm.socketCreate());
    EXPECT_TRUE(sm.safeBind(PORT, "127.0.0.1"));
    EXPECT_TRUE(sm.safeListen(10));
    EXPECT_EQ(sm.setNonBlockingServer(sm.getServerSocket()), 0);
}
