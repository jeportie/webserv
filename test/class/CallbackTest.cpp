/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackTest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:25:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:25:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Callback.hpp"
#include "../../src/class/CallbackQueue.hpp"

// A simple test callback that increments a counter
class TestCallback : public Callback {
public:
    TestCallback(int fd, int* counter)
        : Callback(fd), counter_(counter) {}
    void execute() {
        if (counter_) (*counter_)++;
    }
private:
    int* counter_;
};

TEST(CallbackTest, GetFd) {
    int fd = 123;
    TestCallback cb(fd, nullptr);
    EXPECT_EQ(cb.getFd(), fd);
}

TEST(CallbackQueueTest, PushAndProcess) {
    CallbackQueue queue;
    int counter = 0;
    // Push 3 callbacks
    queue.push(new TestCallback(1, &counter));
    queue.push(new TestCallback(2, &counter));
    queue.push(new TestCallback(3, &counter));
    EXPECT_EQ(queue.size(), 3u);
    queue.processCallbacks();
    EXPECT_EQ(counter, 3);
    EXPECT_TRUE(queue.isEmpty());
}

TEST(CallbackQueueTest, DestructorCleansUp) {
    int counter = 0;
    {
        CallbackQueue queue;
        queue.push(new TestCallback(1, &counter));
        queue.push(new TestCallback(2, &counter));
        // Do not process callbacks, let destructor clean up
    }
    // No crash means success
    SUCCEED();
}
