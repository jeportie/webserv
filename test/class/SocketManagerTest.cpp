/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManagerTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:02:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:03:12 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/SocketManager.hpp"
#include "../../src/class/ClientSocket.hpp"
#include "../../src/class/Callback.hpp"
#include "../../src/class/CallbackQueue.hpp"

// Dummy callback for testing
class DummyCallback : public Callback {
public:
    DummyCallback(int fd, int* counter) : Callback(fd), counter_(counter) {}
    void execute() {
        if (counter_) (*counter_)++;
    }
private:
    int* counter_;
};

TEST(SocketManagerTest, ConstructionAndServerSocketAccess) {
    SocketManager manager;
    // Just check that getServerSocket returns a reference
    ServerSocket& server = manager.getServerSocket();
    (void)server;
    SUCCEED();
}

TEST(SocketManagerTest, AddClientSocket) {
    SocketManager manager;
    int fd = 42;
    ClientSocket* client = new ClientSocket();
    manager.addClientSocket(fd, client);
    // No direct getter, but cleanupClientSocket should delete it
    manager.cleanupClientSocket(fd, -1);
    SUCCEED();
}

TEST(SocketManagerTest, CallbackQueueIntegration) {
    SocketManager manager;
    CallbackQueue& queue = manager.getCallbackQueue();
    int counter = 0;
    queue.push(new DummyCallback(1, &counter));
    queue.push(new DummyCallback(2, &counter));
    queue.processCallbacks();
    EXPECT_EQ(counter, 2);
}

TEST(SocketManagerTest, EpollFdCreation) {
    int epoll_fd = epoll_create1(0);
    ASSERT_NE(epoll_fd, -1);
    close(epoll_fd);
}

