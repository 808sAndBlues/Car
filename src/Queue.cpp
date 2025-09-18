#include "Queue.h"

Queue::Queue(const char* file_destination)
{
    init_mutexes();

    // TODO: Check for failure
    _file_destination = file_destination;
    
    _fstreamer.open(_file_destination, MODE);

    if (!_fstreamer.is_open() || file_operation_failed()) {
        std::cout << "Could not open file";
        std::perror("is_open");
        std::exit(-1);
    }
}

void Queue::init_mutexes()
{
    pthread_mutex_init(&_mutex, nullptr);
    pthread_mutex_init(&_flush_mutex, nullptr);
}

Queue::~Queue()
{
    _fstreamer.close();
    pthread_mutex_destroy(&_mutex);
    pthread_mutex_destroy(&_flush_mutex);
}

void Queue::append(const char *msg)
{
    add_msg(msg);
}

void Queue::add_msg(const char* msg)
{
    // TODO: Depending on severity, we may want to call 'trylock' or 'lock'
    // on the mutex...
    lock_mutex(&_mutex);

    while (_message_count < QUEUE_LEN && *msg != '\0') {
        _queue[_message_count] = *msg;
        
        ++msg;
        ++_message_count;
    }

    if (_message_count >= QUEUE_LEN) {
        flush();
    }

    _queue[_message_count] = '\n';
    ++_message_count;

    unlock_mutex(&_mutex);
}

void Queue::lock_mutex(pthread_mutex_t* mutex)
{
    if (pthread_mutex_lock(mutex) != 0) {
        std::cout << "Error trying to lock mutex!\n";
        std::perror("pthread_mutex_lock");
        std::exit(-1); 
    }
}

void Queue::unlock_mutex(pthread_mutex_t* mutex)
{
    if (pthread_mutex_unlock(mutex) != 0) {
        std::cout << "Error trying to lock mutex!\n";
        std::perror("pthread_mutex_unlock");
        std::exit(-1);
    }
}

void Queue::flush()
{
    lock_mutex(&_flush_mutex); 

    for (std::uint16_t i = 0; i < _message_count; ++i) {    
        _fstreamer << _queue[i];
    }

    _fstreamer.flush();

    reset();

    unlock_mutex(&_flush_mutex);
}

void Queue::reset()
{
    _message_count = 0;
    std::memset(_queue, 0, sizeof(char*) * MAX_MSG_COUNT);
}

std::uint16_t Queue::get_msg_count()
{
    return _message_count;
}

bool Queue::file_operation_failed()
{
    return _fstreamer.fail();
}

