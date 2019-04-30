// vim:ts=4:sts=4:sw=4:expandtab

// (c) Bharanee Rathna 2012

#include "beanstalk.hpp"
#include "test.h"
#include <stdexcept>
#include <string>

using namespace std;
using namespace Beanstalk;

TEST(CONNECTION, CONNECT) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.ping());
    ASSERT_TRUE(client.is_connected());
    ASSERT_TRUE(client.disconnect());
    ASSERT_FALSE(client.disconnect());
    ASSERT_FALSE(client.is_connected());
    return 0;
}

TEST(CONNECTION, RECONNECT) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.ping());
    ASSERT_THROW(client.connect("127.0.0.1", 11300, 2.0), ConnectException);
    ASSERT_NO_THROW(client.reconnect());
    ASSERT_TRUE(client.ping());
    return 0;
}

TEST(CONNECTION, FAILURE) {
    ASSERT_THROW(Client borked("example.org", 3000, 1.0), TimeoutException);
    return 0;
}

TEST(JOB, WATCH_USE_IGNORE) {
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.watch("test"));
    ASSERT_TRUE(client.use("test"));
    ASSERT_TRUE(client.ignore("default"));
    return 0;
}

TEST(JOB, PUT_PEEK_RESERVE_DELETE) {
    Job job;
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.watch("test"));
    ASSERT_TRUE(client.use("test"));
    EXPECT_GT(client.put("hello world!", 0, 0, 2), 0, "put job");
    ASSERT_TRUE(client.peek_ready(job));
    EXPECT_EQ(job.body(), "hello world!", "check peek job");
    ASSERT_TRUE(client.reserve(job));
    EXPECT_EQ(job.body(), "hello world!", "check reserve job");
    ASSERT_TRUE(client.del(job));
    return 0;
}

TEST(JOB, RESERVE_TIMEOUT) {
    Job job;
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.watch("test_timeout"));
    ASSERT_TRUE(client.use("test_timeout"));
    ASSERT_THROW(client.reserve(job, 1), TimeoutException);
    return 0;
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
        ASSERT_TRUE(client2.del(job.id()));
    }

    for (int i = 0; i < 1000; i++) {
        client3.reserve(job);
        EXPECT_EQ(job.body(), message, "check job body");
    }
    return 0;
}

TEST(JOB, INITIAL_READ_BUFFER) {
    Job job;
    Client client("127.0.0.1", 11300);
    ASSERT_TRUE(client.use("test"));
    ASSERT_TRUE(client.watch("test"));

    for (int i = 480; i < 500; i++) {
      string message(i, 'x');

      client.put(message);

      ASSERT_TRUE(client.reserve(job));
      ASSERT_TRUE(client.del(job.id()));
    }
    client.disconnect();
    return 0;
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "================================================================================\n");
    fprintf(stderr, "C++ API\n");
    fprintf(stderr, "================================================================================\n");

    RUN(CONNECTION, CONNECT);
    RUN(CONNECTION, RECONNECT);
    RUN(CONNECTION, FAILURE);
    RUN(JOB, WATCH_USE_IGNORE);
    RUN(JOB, PUT_PEEK_RESERVE_DELETE);
    RUN(JOB, RESERVE_TIMEOUT);
    RUN(JOB, MULTIPLE_LARGE_MESSAGES);
    RUN(JOB, INITIAL_READ_BUFFER);
    return _test_failures;
}
