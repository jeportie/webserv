/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimerTest.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:40:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:40:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Timer.hpp"
#include <ctime>
#include <queue>

// A simple test callback that increments a counter
class TestCallback : public Callback {
public:
    TestCallback(int fd, int* counter) : Callback(fd), counter_(counter) {}
    void execute() {
        if (counter_) (*counter_)++;
    }
private:
    int* counter_;
};

TEST(TimerTest, ConstructorAndGetters) {
    time_t testTime = time(NULL) + 60;
    int counter = 0;
    Callback* cb = new TestCallback(42, &counter);
    Timer timer(testTime, cb);
    EXPECT_EQ(timer.getExpireTime(), testTime);
    EXPECT_EQ(timer.getCallback()->getFd(), 42);
}

TEST(TimerTest, CallbackExecution) {
    time_t now = time(NULL);
    int counter = 0;
    Callback* cb = new TestCallback(1, &counter);
    Timer timer(now, cb);
    timer.getCallback()->execute();
    EXPECT_EQ(counter, 1);
}

TEST(TimerTest, TimerPriorityQueueOrder) {
    time_t now = time(NULL);
    int dummy = 0;
    Timer t1(now + 10, new TestCallback(1, &dummy));
    Timer t2(now + 5, new TestCallback(2, &dummy));
    std::priority_queue<Timer> pq;
    pq.push(t1);
    pq.push(t2);
    // t2 should come out first (earliest expire time)
    EXPECT_EQ(pq.top().getExpireTime(), t2.getExpireTime());
}

TEST(TimerTest, DestructorDeletesCallback) {
    int counter = 0;
    Callback* cb = new TestCallback(1, &counter);
    {
        Timer timer(time(NULL), cb);
        // Do not execute callback, just let timer go out of scope
    }
    // No crash means success
    SUCCEED();
}

