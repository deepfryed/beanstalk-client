// vim:ts=4:sts=4:sw=4:expandtab

// (c) Bharanee Rathna 2012

#include "beanstalk.h"
#include "test.h"
#include <sys/select.h>
#include <fcntl.h>
#include <string>

char *large_message;
int handle, lines = 200, bytes = lines * 27 + 1;

int select_poll(int rw, int fd) {
    fd_set read_set, write_set;
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    if ((rw & 1) == 1)
        FD_SET(fd, &read_set);
    if ((rw & 2) == 2)
        FD_SET(fd, &write_set);

    return select(fd + 1, &read_set, &write_set, 0, 0) > 0 ? 1 : 0;
}

void setup() {
    int i, j, n = 0;
    large_message = (char*)calloc(1, bytes);
    if (!large_message) {
        perror("failed to allocate memory for message");
        exit(1);
    }

    // setup message
    for (i = 0; i < lines; i++) {
        for (j = 0; j < 26; j++)
          large_message[n++] = (char)(j + 97);
        large_message[n++] = '\n';
    }
}

void teardown() {
    free(large_message);
}


TEST(CONNECTION, CONNECT) {
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_disconnect(handle), BS_STATUS_OK, "disconnect");
    handle = bs_connect((char*)"127.0.0.1", 1);
    EXPECT_LT(handle, 0, "disconnect on disconnected handle");
    return 0;
}

TEST(TUBE, USE) {
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_use(handle, (char*)"test"), BS_STATUS_OK, "use tube test");
    bs_disconnect(handle);
    return 0;
}

TEST(TUBE, WATCH) {
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_watch(handle, (char*)"test"), BS_STATUS_OK, "watch tube test");
    bs_disconnect(handle);
    return 0;
}

TEST(TUBE, IGNORE) {
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_watch(handle,  (char*)"test"),    BS_STATUS_OK, "watch tube test");
    EXPECT_EQ(bs_ignore(handle, (char*)"default"), BS_STATUS_OK, "ignore tube default");
    EXPECT_EQ(bs_ignore(handle, (char*)"test"),    BS_STATUS_FAIL, "ignore tube test");
    bs_disconnect(handle);
    return 0;
}

TEST(JOB, PUT) {
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_use(handle, (char*)"test"), BS_STATUS_OK, "use tube test");
    EXPECT_GT(bs_put(handle, 1, 0, 1, (char*)"hello", 5), 0, "put job");
    bs_disconnect(handle);
    return 0;
}

TEST(JOB, RESERVE_RELEASE_DELETE) {
    BSJ *job;
    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_watch(handle, (char*)"test"), BS_STATUS_OK, "watch tube test");
    EXPECT_EQ(bs_reserve(handle, &job), BS_STATUS_OK, "reserve job");
    EXPECT_EQ(std::string(job->data, job->size), "hello", "match against previously put job");
    EXPECT_EQ(bs_release(handle, job->id, 1, 0), BS_STATUS_OK, "release job");
    EXPECT_EQ(bs_delete(handle, job->id), BS_STATUS_OK, "delete job");
    bs_free_job(job);
    bs_disconnect(handle);
    return 0;
}

TEST(JOB, LARGE_CHUNK) {
    BSJ *job;

    handle = bs_connect((char*)"127.0.0.1", 11300);
    EXPECT_GT(handle, 0, "connect");
    EXPECT_EQ(bs_use(handle,   (char*)"test"), BS_STATUS_OK, "use tube test");
    EXPECT_EQ(bs_watch(handle, (char*)"test"), BS_STATUS_OK, "watch tube test");
    EXPECT_GT(bs_put(handle, 1, 0, 1, large_message, bytes), 0, "put job");
    EXPECT_EQ(bs_reserve(handle, &job), BS_STATUS_OK, "reserve job");
    EXPECT_EQ(job->size, bytes, "check job size");
    EXPECT_EQ(memcmp(job->data, large_message, bytes), 0, "check job data");
    EXPECT_EQ(bs_delete(handle, job->id), BS_STATUS_OK, "delete job");

    bs_free_job(job);
    bs_disconnect(handle);
    return 0;
}

TEST(POLL, SELECT) {
    BSJ *job;

    handle = bs_connect((char*)"127.0.0.1", 11300);

    EXPECT_GT(handle, 0, "connect");
    bs_start_polling(select_poll);
    fcntl(handle, F_SETFL, O_NONBLOCK);
    EXPECT_EQ(bs_use(handle,   (char*)"test"), BS_STATUS_OK, "use tube test");
    EXPECT_EQ(bs_watch(handle, (char*)"test"), BS_STATUS_OK, "watch tube");
    EXPECT_GT(bs_put(handle, 1, 0, 1, large_message, bytes), 0, "put job");
    EXPECT_EQ(bs_reserve(handle, &job), BS_STATUS_OK, "reserve job");
    EXPECT_EQ(job->size, bytes, "check job size");
    EXPECT_EQ(memcmp(job->data, large_message, bytes), 0, "check job data");
    EXPECT_EQ(bs_delete(handle, job->id), BS_STATUS_OK, "delete job");

    bs_free_job(job);
    bs_disconnect(handle);
    return 0;
}

int main(int argc, char *argv[]) {
    setup();

    fprintf(stderr, "================================================================================\n");
    fprintf(stderr, "C API\n");
    fprintf(stderr, "================================================================================\n");

    RUN(CONNECTION, CONNECT);
    RUN(TUBE, USE);
    RUN(TUBE, WATCH);
    RUN(TUBE, IGNORE);
    RUN(JOB, PUT);
    RUN(JOB, RESERVE_RELEASE_DELETE);
    RUN(JOB, LARGE_CHUNK);
    RUN(POLL, SELECT);

    teardown();
    return _test_failures;
}
