#include "Logger.h"

void Logger::main_loop()
{
    while (!_kill_flag.get_kill()) {
    }
}


void* logger_main(void *obj)
{
    Logger* logger = (Logger*) obj;
    logger->main_loop();

    return nullptr;
}

void Logger::log_debug(char* msg)
{
    _queue.append(msg);
}

void Logger::flush_log()
{
    _queue.flush();
}

