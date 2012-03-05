#include "beanstalk.hpp"
#include <stdexcept>

using namespace std;

namespace Beanstalk {

    Job::Job(int id, char *data, size_t size) {
        _body.assign(data, size);
        _id = id;
    }

    Job::Job(BSJ *job) {
        if (job) {
            _body.assign(job->data, job->size);
            _id = job->id;
            bs_free_job(job);
        }
        else {
            _id = 0;
        }
    }

    bool Job::is_valid() {
        return _id > 0;
    }

    string& Job::body() {
        return _body;
    }

    int Job::id() {
        return _id;
    }

    Client::~Client() {
        bs_disconnect(handle);
    }

    Client::Client(string host, int port) {
        handle = bs_connect((char*)host.c_str(), port);
        if (handle < 0)
            throw new runtime_error("unable to connect to beanstalkd at " + host);
    }

    bool Client::use(string tube) {
        return bs_use(handle, (char*)tube.c_str()) == BS_STATUS_OK;
    }

    bool Client::watch(string tube) {
        return bs_watch(handle, (char*)tube.c_str()) == BS_STATUS_OK;
    }

    bool Client::ignore(string tube) {
        return bs_ignore(handle, (char*)tube.c_str()) == BS_STATUS_OK;
    }

    int Client::put(string body, int priority, int delay, int ttr) {
        int id = bs_put(handle, priority, delay, ttr, (char*)body.data(), body.size());
        return (id > 0 ? id : 0);
    }

    int Client::put(char *body, size_t bytes, int priority, int delay, int ttr) {
        int id = bs_put(handle, priority, delay, ttr, body, bytes);
        return (id > 0 ? id : 0);
    }

    bool Client::del(int id) {
        return bs_delete(handle, id) == BS_STATUS_OK;
    }

    BSJ* Client::reserve() {
        BSJ *job;
        return (bs_reserve(handle, &job) == BS_STATUS_OK ? job : 0);
    }

    BSJ* Client::reserve(int timeout) {
        BSJ *job;
        return (bs_reserve_with_timeout(handle, timeout, &job) == BS_STATUS_OK ? job : 0);
    }

    bool Client::release(int id, int priority, int delay) {
        return bs_release(handle, id, priority, delay) == BS_STATUS_OK;
    }

    bool Client::bury(int id, int priority) {
        return bs_bury(handle, id, priority) == BS_STATUS_OK;
    }

    bool Client::touch(int id) {
        return bs_touch(handle, id) == BS_STATUS_OK;
    }

    BSJ* Client::peek(int id) {
        BSJ *job;
        return (bs_peek(handle, id, &job) == BS_STATUS_OK ? job : 0);
    }

    BSJ* Client::peek_ready() {
        BSJ *job;
        return (bs_peek_ready(handle, &job) == BS_STATUS_OK ? job : 0);
    }

    BSJ* Client::peek_delayed() {
        BSJ *job;
        return (bs_peek_delayed(handle, &job) == BS_STATUS_OK ? job : 0);
    }

    BSJ* Client::peek_buried() {
        BSJ *job;
        return (bs_peek_buried(handle, &job) == BS_STATUS_OK ? job : 0);
    }

    bool Client::kick(int bound) {
        return bs_kick(handle, bound) == BS_STATUS_OK;
    }
}
