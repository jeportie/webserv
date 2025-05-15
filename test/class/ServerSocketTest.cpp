/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocketTest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:15:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:15:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/ServerSocket.hpp"

TEST(ServerSocketTest, DefaultConstructor)
{
    ServerSocket socket;
    EXPECT_FALSE(socket.isValid());
    EXPECT_FALSE(socket.isNonBlocking());
    EXPECT_EQ(socket.getFd(), -1);
}
