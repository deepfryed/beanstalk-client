#include "beanstalk.h"
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

int main() {
    BSJ *job;
    char *yaml;
    int64_t id;
    int a,b,c;

    bs_version(&a, &b, &c);
    printf("beanstalk-client version %d.%d.%d\n", a,b,c);

    int socket = bs_connect("127.0.0.1", 11300);

    assert(socket != BS_STATUS_FAIL);
    assert(bs_use(socket,    "test")    == BS_STATUS_OK);
    assert(bs_watch(socket,  "test")    == BS_STATUS_OK);
    assert(bs_ignore(socket, "default") == BS_STATUS_OK);

    id = bs_put(socket, 0, 0, 60, "hello world", 11);
    assert(id > 0);
    printf("put job id: %"PRId64"\n", id);

    assert(bs_reserve_with_timeout(socket, 2, &job) == BS_STATUS_OK);
    assert(job);

    printf("reserve job id: %"PRId64" size: %lu\n", job->id, job->size);
    write(fileno(stderr), job->data, job->size);
    write(fileno(stderr), "\r\n", 2);

    printf("release job id: %"PRId64"\n", job->id);
    assert(bs_release(socket, job->id, 0, 0) == BS_STATUS_OK);

    bs_free_job(job);
    assert(bs_peek_ready(socket, &job) == BS_STATUS_OK);
    printf("peek job id: %"PRId64"\n", job->id);

    printf("delete job id: %"PRId64"\n", job->id);
    assert(bs_delete(socket, job->id) == BS_STATUS_OK);
    bs_free_job(job);

    assert(bs_list_tubes(socket, &yaml) == BS_STATUS_OK);
    printf("tubes:\n%s\n", yaml);
    free(yaml);
    assert(bs_list_tubes_watched(socket, &yaml) == BS_STATUS_OK);
    printf("tubes-watched:\n%s\n", yaml);
    free(yaml);
    assert(bs_list_tube_used(socket, &yaml) == BS_STATUS_OK);
    printf("tube-used: %s\n", yaml);
    free(yaml);
    assert(bs_stats(socket, &yaml) == BS_STATUS_OK);
    printf("stats:\n%s\n", yaml);
    free(yaml);
    bs_disconnect(socket);
    exit(0);
}
