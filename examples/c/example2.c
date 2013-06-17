#include "beanstalk.h"
#include <stdio.h>
#include <assert.h>
#include <sys/select.h>
#include <fcntl.h>
#include <inttypes.h>

// example 2: polling the socket descriptor

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

int main() {
    BSJ *job;
    int64_t id;
    int handle = bs_connect("127.0.0.1", 11300);

    assert(handle != BS_STATUS_FAIL);
    assert(bs_use(handle,    "test")    == BS_STATUS_OK);
    assert(bs_watch(handle,  "test")    == BS_STATUS_OK);
    assert(bs_ignore(handle, "default") == BS_STATUS_OK);

    id = bs_put(handle, 0, 0, 60, "hello world", 11);
    assert(id > 0);
    printf("put job id: %"PRId64"\n", id);

    // poll read & writes from now on.
    bs_start_polling(select_poll);
    fcntl(handle, F_SETFL, fcntl(handle, F_GETFL) | O_NONBLOCK);

    assert(bs_reserve_with_timeout(handle, 2, &job) == BS_STATUS_OK);
    assert(job);

    printf("reserve job id: %"PRId64" size: %lu\n", job->id, job->size);
    write(fileno(stderr), job->data, job->size);
    write(fileno(stderr), "\r\n", 2);

    bs_reset_polling();
    fcntl(handle, F_SETFL, fcntl(handle, F_GETFL) ^ O_NONBLOCK);
    printf("delete job id: %"PRId64"\n", job->id);
    assert(bs_delete(handle, job->id) == BS_STATUS_OK);
    bs_free_job(job);
    bs_disconnect(handle);
    exit(0);
}
