#include "beanstalk.hpp"
#include <stdexcept>

using namespace std;

namespace Beanstalk {

    Job::Job() {
        _id = 0;
    }

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
            throw runtime_error("unable to connect to beanstalkd at " + host);
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

    bool Client::reserve(Job &job) {
        BSJ *bsj;
        if (bs_reserve(handle, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
    }

    bool Client::reserve(Job &job, int timeout) {
        BSJ *bsj;
        if (bs_reserve_with_timeout(handle, timeout, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
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

    bool Client::peek(Job &job, int id) {
        BSJ *bsj;
        if (bs_peek(handle, id, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
    }

    bool Client::peek_ready(Job &job) {
        BSJ *bsj;
        if (bs_peek_ready(handle, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
    }

    bool Client::peek_delayed(Job &job) {
        BSJ *bsj;
        if (bs_peek_delayed(handle, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
    }

    bool Client::peek_buried(Job &job) {
        BSJ *bsj;
        if (bs_peek_buried(handle, &bsj) == BS_STATUS_OK) {
            job = bsj;
            return true;
        }
        return false;
    }

    bool Client::kick(int bound) {
        return bs_kick(handle, bound) == BS_STATUS_OK;
    }
}
