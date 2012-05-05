// vim:ts=4:sts=4:sw=4:expandtab

// (c) Bharanee Rathna 2012

#include "beanstalk.hpp"
#include "gtest/gtest.h"
#include <stdexcept>
#include <string>

using namespace std;
using namespace Beanstalk;

TEST(Connection, Connect) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.ping());
    ASSERT_TRUE(client.disconnect());
    ASSERT_FALSE(client.disconnect());
}

TEST(Connection, Reconnect) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.ping());
    ASSERT_THROW(client.connect("127.0.0.1", 11300), runtime_error);
    ASSERT_NO_THROW(client.reconnect());
    ASSERT_TRUE(client.ping());
}

TEST(Connection, Failure) {
    ASSERT_THROW(Client borked("123.456.789.0", 0), runtime_error);
}

TEST(JOB, WATCH_USE_IGNORE) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.watch("test"));
    ASSERT_TRUE(client.use("test"));
    ASSERT_TRUE(client.ignore("default"));
}

TEST(JOB, PUT_PEEK_RESERVE_DELETE) {
    Job job;
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.watch("test"));
    ASSERT_TRUE(client.use("test"));
    EXPECT_GT(client.put("hello world!", 0, 0, 2), 0);
    ASSERT_TRUE(client.peek_ready(job));
    EXPECT_EQ(job.body(), "hello world!");
    ASSERT_TRUE(client.reserve(job));
    EXPECT_EQ(job.body(), "hello world!");
    ASSERT_TRUE(client.del(job.id()));
}

TEST(JOB, MULTIPLE_LARGE_MESSAGES) {
    Job job;
    Client client1("127.0.0.1", 11300);
    Client client2("127.0.0.1", 11300);
    Client client3("127.0.0.1", 11300);
    ASSERT_TRUE(client1.use("test1"));
    ASSERT_TRUE(client2.watch("test1"));
    ASSERT_TRUE(client2.use("test2"));
    ASSERT_TRUE(client3.watch("test2"));

    string message = "12345";
    for (int i = 0; i < 5000; i++)
        message += ", 12345";

    for (int i = 0; i < 1000; i++)
        client1.put(message);

    for (int i = 0; i < 1000; i++) {
        client2.reserve(job);
        client2.put(job.body());
        client2.del(job.id());
    }

    for (int i = 0; i < 1000; i++) {
        client3.reserve(job);
        ASSERT_EQ(job.body(), message);
    }
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
