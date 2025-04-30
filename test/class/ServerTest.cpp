/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsalomon <fsalomon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:06:38 by jeportie          #+#    #+#             */
/*   Updated: 2025/04/30 17:53:16 by fsalomon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "../../src/class/Server.hpp"

TEST(ServerTest, DefaultConstructor)
{
    Server o;

	EXPECT_EQ(o.getServerSocket(), -1);
	EXPECT_EQ(o.getClientSocket(), -1);
}


TEST(ServerTest, InitConnect)
{
    Server o;

    ASSERT_NE(o.getServerSocket(), -1);
	
	EXPECT_EQ(o.safeBind(), true);
	EXPECT_EQ(o.safeListen(), true);
	EXPECT_EQ(o.safeAccept(), true);

    ASSERT_NE(o.getClientSocket(), -1);

	// Start to listen std out
    testing::internal::CaptureStdout();
    o.safeAccept();
    std::string out = testing::internal::GetCapturedStdout();
    EXPECT_EQ(out, "Connection accepted!");
}
