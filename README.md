beanstalkd client
=================

C/C++ [beanstalkd](http://kr.github.com/beanstalkd) client that is blocking by default.


## Install

```
sudo make install
```

## Testing

```
make test
```

## C Example

```C

// compile as: gcc -o example example.c -lbeanstalk

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

## C++ Example

```C++

// compile as: g++ -o example example.cc -lbeanstalk

#include "beanstalk.hpp"
#include <iostream>
#include <assert.h>

using namespace std;
using namespace Beanstalk;

int main() {
    Client client("127.0.0.1", 11300);
    assert(client.use("test"));
    assert(client.watch("test"));

    int id = client.put("hello");
    assert(id > 0);
    cout << "put job id: " << id << endl;

    Job job;
    assert(client.reserve(job) && job);
    assert(job.id() == id);

    cout << "reserved job id: "
         << job.id()
         << " with body {" << job.body() << "}"
         << endl;

    assert(client.del(job.id()));
    cout << "deleted job id: " << job.id() << endl;
}
```

## Changelog

See `debian/changelog`

## Contributing

1. Fork / clone the repo.
2. Make your changes.
3. Make sure to add a test for any code changes.
4. Make sure the tests pass.
5. Send a pull request or a patch with explanation.

## License

MIT
