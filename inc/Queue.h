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

class Queue
{
    private:
        std::uint16_t _message_count = 0;
        char _queue[QUEUE_LEN] = {0};

        std::ofstream _fstreamer;

        char* _file_destination = nullptr;

        void reset();
        void add_msg(char* msg);

        void flush();

    public:
        Queue(char* file_destination);
        ~Queue();
        
        void append(char *msg);




};

#endif
