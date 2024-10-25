#ifndef QUEUE_H
#define QUEUE_H

#include <sys/timerfd.h>
#include <cstdint>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <cstring>

#define MAX_MSG_COUNT 100
#define MAX_MSG_LEN   256
#define QUEUE_LEN     MAX_MSG_COUNT * MAX_MSG_LEN
#define DEQUEUE_INTERVAL 1.0

const std::ofstream::openmode MODE = std::ofstream::out | std::ofstream::trunc;

class Queue
{
    private:
        std::uint16_t _message_count = 0;
        char _queue[QUEUE_LEN] = {0};

        std::ofstream _fstreamer;

        pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

        char* _file_destination = nullptr;

        void reset();
        void add_msg(char* msg);


    public:
        Queue(char* file_destination);
        ~Queue();
        
        void append(char *msg);
        void flush();




};

#endif
