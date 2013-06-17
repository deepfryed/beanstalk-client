#include "beanstalk.hpp"
#include <iostream>
#include <assert.h>
#include <stdexcept>

using namespace std;
using namespace Beanstalk;

int main() {
    Client client("127.0.0.1", 11300);
    assert(client.use("test"));
    assert(client.watch("test"));
    assert(client.ignore("default"));

    int64_t id = client.put("hello");
    assert(id > 0);
    cout << "put job id: " << id << endl;

    Job job;
    assert(client.peek_ready(job) && job);
    cout << "peek job id: " << job.id() << endl;

    assert(client.reserve(job) && job);
    assert(job.id() == id);
    cout << "reserved job id: "
         << job.id()
         << " with body {" << job.body() << "}"
         << endl;

    assert(client.del(job.id()));
    cout << "deleted job id: " << job.id() << endl;

    info_list_t tubes = client.list_tubes();
    info_hash_t stats = client.stats();

    // failed connnection
    try {
        cout << "testing connection error" << endl;
        Client client("123.456.789.0", 1234);
    }
    catch (runtime_error &e) {
        cerr << e.what() << endl;
    }
}
