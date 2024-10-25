#include "Queue.h"

Queue::Queue(char* file_destination)
{
    // Init mutex
    pthread_mutex_init(&_mutex, nullptr);

    // TODO: Setup output file
    _file_destination = file_destination;
    
    _fstreamer.open(_file_destination, MODE);
    if (!_fstreamer.is_open()) {
        std::cout << "Could not open file";
        std::perror("is_open");
        std::exit(-1);
    }
}

Queue::~Queue()
{
    _fstreamer.close();
    pthread_mutex_destroy(&_mutex);
}

void Queue::append(char *msg)
{
    add_msg(msg);
}

void Queue::add_msg(char* msg)
{
    // TODO: Depending on severity, we may want to call 'trylock' or 'lock'
    // on the mutex...

    if (pthread_mutex_lock(&_mutex) != 0) {
        std::cout << "Error trying to lock mutex!\n";
        std::perror("pthread_mutex_lock");
        std::exit(-1); 
    }

    while (_message_count < QUEUE_LEN && *msg != '\0') {
        _queue[_message_count] = *msg;
        
        ++msg;
        ++_message_count;
    }

    if (pthread_mutex_unlock(&_mutex) != 0) {
        std::cout << "Failed to unlock mutex!\n";
        std::perror("pthread_mutex_unlock");
        std::exit(-1);
    }


    if (_message_count >= QUEUE_LEN) {
        // TODO: Flush the buffer
        flush();
    }

    if (pthread_mutex_lock(&_mutex) != 0) {
        std::cout << "Error trying to lock mutex!\n";
        std::perror("pthread_mutex_lock");
        std::exit(-1); 
    }
    
    // Add '\n' character to output
    _queue[_message_count] = '\n';
    ++_message_count;

    if (pthread_mutex_unlock(&_mutex) != 0) {
        std::cout << "Failed to unlock mutex!\n";
        std::perror("pthread_mutex_unlock");
        std::exit(-1);
    }
}

void Queue::flush()
{
    
    if (pthread_mutex_lock(&_mutex) != 0) {
        std::cout << "Error trying to lock mutex!\n";
        std::perror("pthread_mutex_lock");
        std::exit(-1); 
    }

    for (std::uint16_t i = 0; i < _message_count; ++i) {    
        _fstreamer << _queue[i];
    }

    reset();

    if (pthread_mutex_unlock(&_mutex) != 0) {
        std::cout << "Failed to unlock mutex!\n";
        std::perror("pthread_mutex_unlock");
        std::exit(-1);
    }
}

void Queue::reset()
{
    // TODO: Verify that _queue is not set to nullptr after memset
    _message_count = 0;
    std::memset(_queue, 0, sizeof(char*) * MAX_MSG_COUNT);
}
