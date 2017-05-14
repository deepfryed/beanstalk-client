#pragma once

#include "beanstalk.h"
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <utility>

namespace Beanstalk {

    typedef std::vector<std::string> info_list_t;
    typedef std::map<std::string, std::string> info_hash_t;

    class Job {
        public:
            int64_t id() const;
            std::string& body();
            Job(int64_t id, const char* data, size_t size);
            Job(BSJ*);
            Job();
            operator bool() { return _id > 0; }
        protected:
            int64_t _id;
            std::string _body;
    };

    class Exception: public std::runtime_error {
        public:
            Exception(std::string message)
                : std::runtime_error(std::move(message))
            {}
    };

    class ConnectException: public Exception {
        public:
            ConnectException(std::string message)
                : Exception(std::move(message))
            {}
            ConnectException(std::string host, uint16_t port)
                : ConnectException("unable to connect to Beanstalk server ", host, port)
            {}
            ConnectException(std::string error, std::string host, uint16_t port)
                : ConnectException(error + host_str(host, port))
            {}
        private:
            static std::string host_str(std::string host, uint16_t port) {
                return host + ":" + std::to_string(port);
            }
    };

    class TimeoutException: public Exception {
        public:
            TimeoutException(std::string operation)
                : Exception("operation '" + operation + "' timed out")
            {}
    };

    class Client {
        public:
            ~Client();
            Client();
            Client(std::string host, uint16_t port, float timeout_secs = 0);
            bool ping();
            bool use(const std::string& tube);
            bool watch(const std::string& tube);
            bool ignore(const std::string& tube);
            int64_t put(const std::string& body, uint32_t priority = 0, uint32_t delay = 0, uint32_t ttr = 60);
            int64_t put(const char *data, size_t bytes, uint32_t priority, uint32_t delay, uint32_t ttr);
            bool del(int64_t id);
            bool del(const Job &job);
            bool reserve(Job &job);
            bool reserve(Job &job, uint32_t timeout);
            bool release(const Job &job, uint32_t priority = 1, uint32_t delay = 0);
            bool release(int64_t id, uint32_t priority = 1, uint32_t delay = 0);
            bool bury(const Job &job, uint32_t priority = 1);
            bool bury(int64_t id, uint32_t priority = 1);
            bool touch(const Job &job);
            bool touch(int64_t id);
            bool peek(Job &job, int64_t id);
            bool peek_ready(Job &job);
            bool peek_delayed(Job &job);
            bool peek_buried(Job &job);
            bool kick(int bound);
            void connect(const std::string& host, uint16_t port, float timeout_secs = 0);
            void reconnect();
            bool disconnect();
            bool is_connected();
            std::string list_tube_used();
            info_list_t list_tubes();
            info_list_t list_tubes_watched();
            info_hash_t stats();
            info_hash_t stats_job(int64_t id);
            info_hash_t stats_tube(const std::string& name);

            static void version(int *major, int *minor, int *patch);
            static std::string version();

        protected:
            int _handle;
            std::string _host;
            uint16_t _port;
            float _timeout_secs;
    };
}
