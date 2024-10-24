#include "Queue.h"

Queue::Queue(char* file_destination)
{
    // TODO: Setup output file
    _file_destination = file_destination;
    
    _fstreamer.open(_file_destination);
    if (!_fstreamer.is_open()) {
        std::cout << "Could not open file";
    }
    add_msg("Hello!");
    flush();
}

Queue::~Queue()
{
    _fstreamer.close();
}

void Queue::append(char *msg)
{
    if (_message_count >= MAX_MSG_COUNT) {
        reset();
    }

    add_msg(msg);
}

void Queue::add_msg(char* msg)
{
    while (_message_count < QUEUE_LEN && *msg != '\0') {
        _queue[_message_count] = *msg;
        
        ++msg;
        ++_message_count;
    }

    if (_message_count == QUEUE_LEN) {
       // TODO: Flush the buffer 
    }
    
    // Add '\n' character to output
    _queue[_message_count] = '\n';
    ++_message_count;

    std::cout << _queue;
}

void Queue::flush()
{
    for (std::uint16_t i = 0; i < _message_count; ++i) {    
        _fstreamer << _queue[i];
    }
}


void Queue::reset()
{
    // TODO: Verify that _queue is not set to nullptr after memset
    _message_count = 0;
    std::memset(_queue, 0, sizeof(char*) * MAX_MSG_COUNT);
}
