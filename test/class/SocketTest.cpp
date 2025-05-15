/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:10:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:10:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Socket.hpp"

// Since Socket is an abstract class, we need a concrete implementation for testing
class TestSocket : public Socket {
public:
    TestSocket() : Socket() {}
    virtual ~TestSocket() {}
    
    // Implement pure virtual methods
    virtual int setNonBlocking(int fd)
	{ 
		(void) fd;
		return 0; 
	}
    virtual int safeFcntl(int fd, int cmd, int flag)
	{
		(void) fd;
		(void) cmd;
		(void) flag;
		return 0;
	}
};

TEST(SocketTest, DefaultConstructor)
{
    TestSocket socket;
    EXPECT_FALSE(socket.isValid());
    EXPECT_FALSE(socket.isNonBlocking());
    EXPECT_EQ(socket.getFd(), -1);
}
