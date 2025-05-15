/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocketTest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:20:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:20:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/ClientSocket.hpp"

TEST(ClientSocketTest, DefaultConstructor)
{
    ClientSocket socket;
    EXPECT_FALSE(socket.isValid());
    EXPECT_FALSE(socket.isNonBlocking());
    EXPECT_EQ(socket.getFd(), -1);
    EXPECT_EQ(socket.getClientPort(), 0);
    EXPECT_EQ(socket.getClientIP(), "0.0.0.0");
}
