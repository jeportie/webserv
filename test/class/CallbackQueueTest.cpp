/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueueTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:30:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:30:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackQueue.hpp"

TEST(CallbackQueueTest, DefaultConstructor)
{
    CallbackQueue queue;
    EXPECT_TRUE(queue.isEmpty());
    EXPECT_EQ(queue.size(), 0);
}
