#pragma once

#include "beanstalk.h"
#include <string>
#include <vector>
#include <map>

namespace Beanstalk {
    typedef std::vector<std::string> info_list_t;
    typedef std::map<std::string, std::string> info_hash_t;

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
            bool peek(Job &, int id);
            bool peek_ready(Job &);
            bool peek_delayed(Job &);
            bool peek_buried(Job &);
            bool kick(int bound);
            std::string list_tube_used();
            info_list_t list_tubes();
            info_list_t list_tubes_watched();
            info_hash_t stats();
            info_hash_t stats_job(int);
            info_hash_t stats_tube(std::string);
        protected:
            int handle;
    };
}
