#ifndef QUEUE_H
#define QUEUE_H

#include <sys/timerfd.h>
#include <cstdint>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <cstring>

#define MAX_MSG_COUNT    100
#define MAX_MSG_LEN      256
#define QUEUE_LEN        MAX_MSG_COUNT * MAX_MSG_LEN
#define DEQUEUE_INTERVAL 1.0

const std::ofstream::openmode MODE = std::ofstream::out | std::ofstream::trunc;

class Queue
{
    protected:
        std::uint16_t _message_count = 0;
        char _queue[QUEUE_LEN] = {0};

        std::ofstream _fstreamer;

        pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t _flush_mutex = PTHREAD_MUTEX_INITIALIZER;

        const char* _file_destination = nullptr;

        void reset();
        void add_msg(const char* msg);

        void lock_mutex(pthread_mutex_t* mutex);
        void unlock_mutex(pthread_mutex_t* mutex);

        void init_mutexes();

        bool file_operation_failed();

    public:
        Queue(const char* file_destination);
        ~Queue();
        
        void append(const char *msg);
        void flush();

        std::uint16_t get_msg_count();
};

#endif
