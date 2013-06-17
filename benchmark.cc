#include "beanstalk.hpp"
#include <iostream>
#include <assert.h>
#include <stdexcept>
#include <pthread.h>
#include <sys/time.h>

using namespace std;
using namespace Beanstalk;

typedef struct timeval timeval;

#define WINDOW     1000
#define TOTAL_JOBS 100000

double duration(timeval *start, timeval *end) {
    return (double)((end->tv_sec * 1000000 + end->tv_usec) - (start->tv_sec * 1000000 + start->tv_usec)) / 1000000.0;
}

void* producer(void* /*arg*/) {
    Client client("127.0.0.1", 11300);

    timeval start, end;
    gettimeofday(&start, 0);
    for (int i = 0; i < TOTAL_JOBS; i++) {
        client.put("hello world");
        if (i > 0 && i % WINDOW == 0) {
            gettimeofday(&end, 0);
            printf("producer rate: %.lf\n", (double)WINDOW / duration(&start, &end));
            memcpy(&start, &end, sizeof(end));
        }
    }

    return 0;
}

void* consumer(void* /*arg*/) {
    Client client("127.0.0.1", 11300);

    Job job;
    size_t jobs = 0;
    timeval start, end;
    gettimeofday(&start, 0);
    while (jobs < TOTAL_JOBS && client.reserve(job)) {
        jobs++;
        client.del(job.id());
        if (jobs % WINDOW == 0) {
            gettimeofday(&end, 0);
            printf("consumer rate: %.lf\n", (float)WINDOW / duration(&start, &end));
            memcpy(&start, &end, sizeof(end));
        }
    }

    return 0;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    pthread_create(&producer_thread, 0, producer, 0);
    pthread_create(&consumer_thread, 0, consumer, 0);

    pthread_join(producer_thread, 0);
    pthread_join(consumer_thread, 0);

    return 0;
}
