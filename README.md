beanstalkd client
=================

C/C++ [beanstalkd](http://kr.github.com/beanstalkd) client that is blocking by default.


## Install

```
sudo make install
```

## Testing

The tests need [google test](http://code.google.com/p/googletest) installed (libgtest-dev on debian based distros).

```
sudo apt-get install libgtest-dev
make test
```

## C Example

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

## C++ Example

```C++
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

    Job job = client.reserve();
    assert(job.is_valid());
    assert(job.id() == id);

    cout << "reserved job id: "
         << job.id()
         << " with body {" << job.body() << "}"
         << endl;

    assert(client.del(job.id()));
    cout << "deleted job id: " << job.id() << endl;
}
```

## License

[Creative Commons Attribution - CC BY](http://creativecommons.org/licenses/by/3.0)
