beanstalkd C client
===================

This is a blocking version of [http://kr.github.com/beanstalkd/](beanstalkd) client.


## Install

```
sudo make install
```

## Example

```C
#include "beanstalk.h"
#include <stdio.h>
#include <assert.h>

int main() {
    BSJ *job;
    int id, socket = bs_connect("127.0.0.1", 11300);

    assert(socket != BS_STATUS_FAIL);
    assert(bs_use(socket,    "test")    == BS_STATUS_OK);
    assert(bs_watch(socket,  "test")    == BS_STATUS_OK);
    assert(bs_ignore(socket, "default") == BS_STATUS_OK);

    id = bs_put(socket, 0, 0, 60, "hello world", 11);

    assert(id > 0);
    printf("put job id: %d\n", id);

    assert(bs_reserve_with_timeout(socket, 2, &job) == BS_STATUS_OK);
    assert(job);

    printf("reserve job id: %d size: %lu\n", job->id, job->size);
    write(fileno(stderr), job->data, job->size);
    write(fileno(stderr), "\r\n", 2);

    printf("delete job id: %d\n", job->id);
    assert(bs_delete(socket, job->id) == BS_STATUS_OK);
    bs_free_job(job);

    bs_disconnect(socket);
}

```

## Warning

This is a weekend hack, the API might change drastically and some horrendous bugs may get fixed later. Use it at your own
peril. Valgrid seems to be happy so far.

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)
