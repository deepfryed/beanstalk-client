#pragma once

#include "beanstalk.h"
#include <string>

// TODO: stats & list commands.

namespace Beanstalk {
    class Job {
        public:
            int id();
            std::string& body();
            Job(int, char*, size_t);
            Job(BSJ*);
            Job();
            operator bool() { return _id > 0; }
        protected:
            int _id;
            std::string _body;
    };

    class Client {
        public:
            ~Client();
            Client(std::string host, int port);
            bool use(std::string);
            bool watch(std::string);
            bool ignore(std::string);
            int  put(std::string, int priority = 0, int delay = 0, int ttr = 60);
            int  put(char *data, size_t bytes, int priority = 0, int delay = 0, int ttr = 60);
            bool del(int id);
            bool reserve(Job &);
            bool reserve(Job &, int timeout);
            bool release(int id, int priority = 1, int delay = 0);
            bool bury(int id, int priority = 1);
            bool touch(int id);
            BSJ* peek(int id);
            BSJ* peek_ready();
            BSJ* peek_delayed();
            BSJ* peek_buried();
            bool kick(int bound);
        protected:
            int handle;
    };
}
