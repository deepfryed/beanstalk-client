// vim:ts=4:sts=4:sw=4:expandtab

// (c) Bharanee Rathna 2012

#include "beanstalk.hpp"
#include "gtest/gtest.h"
#include <stdexcept>

using namespace Beanstalk;

TEST(Connection, Connect) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.ping());
    ASSERT_TRUE(client.disconnect());
    ASSERT_FALSE(client.disconnect());
}

TEST(Connection, Failure) {
    ASSERT_THROW(Client borked("123.456.789.0", 0), std::runtime_error);
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
    EXPECT_GT(client.put("hello world!"), 0);
    ASSERT_TRUE(client.peek_ready(job));
    EXPECT_EQ(job.body(), "hello world!");
    ASSERT_TRUE(client.reserve(job));
    EXPECT_EQ(job.body(), "hello world!");
    ASSERT_TRUE(client.del(job.id()));
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
