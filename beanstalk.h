#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BS_STATUS_OK             0
#define BS_STATUS_FAIL          -1
#define BS_STATUS_EXPECTED_CRLF -2
#define BS_STATUS_JOB_TOO_BIG   -3
#define BS_STATUS_DRAINING      -4
#define BS_STATUS_TIMED_OUT     -5
#define BS_STATUS_NOT_FOUND     -6
#define BS_STATUS_DEADLINE_SOON -7
#define BS_STATUS_BURIED        -8
#define BS_STATUS_NOT_IGNORED   -9

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct bs_message {
    char *data;
    char *status;
    size_t size;
} BSM;

typedef struct bs_job {
    int id;
    char *data;
    size_t size;
} BSJ;

const char* bs_status_text(int code);

int bs_connect(char *host, int port);
int bs_disconnect(int fd);

void bs_free_message(BSM* m);
void bs_free_job(BSJ *job);

int bs_use(int fd, char *tube);
int bs_watch(int fd, char *tube);
int bs_ignore(int fd, char *tube);
int bs_put(int fd, int priority, int delay, int ttr, char *data, size_t bytes);
int bs_delete(int fd, int job);
int bs_reserve(int fd, BSJ **result);
int bs_reserve_with_timeout(int fd, int ttl, BSJ **result);
int bs_release(int fd, int id, int priority, int delay);
int bs_bury(int fd, int id, int priority);
int bs_touch(int fd, int id);
int bs_peek_job(int fd, char *command, BSJ **result);
int bs_peek(int fd, int id, BSJ **job);
int bs_peek_ready(int fd, BSJ **job);
int bs_peek_delayed(int fd, BSJ **job);
int bs_peek_buried(int fd, BSJ **job);
int bs_kick(int fd, int bound);
int bs_list_tube_used(int fd, char **tube);
int bs_list_tubes(int fd, char **yaml);
int bs_list_tubes_watched(int fd, char **yaml);
int bs_stats(int fd, char **yaml);
int bs_stats_job(int fd, int id, char **yaml);
int bs_stats_tube(int fd, char *tube, char **yaml);

#ifdef __cplusplus
    }
#endif
