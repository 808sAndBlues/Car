#include "Logger.h"

void Logger::main_loop()
{
    std::cout << "I am here\n";
}


void* logger_main(void *obj)
{
    Logger* logger = (Logger*) obj;
    logger->main_loop();

    return nullptr;
}

