
# TODOS

- Need to setup epolling for handling shutdown signals
- Need to finish Logger
    - Requires some special storage to handle concurrent logging
    - Queue? Circular buffer?
    - Need to focus on memory usage and what's practical for this project
- Need to setup GTest
- Pipeline(s) for running tests
- Parts list and requirements doc

# 10-18-24

Created the main classes for the Car project and stood up CMake. Got a lot of things I need to figure out.

# 10-19-24

Standing up a lot more infrastructure in regards to signal handling. Avoiding usage of signal handlers because it is easier to test w/ poll/epolling.

EDGE TRIGGERED VS LEVEL TRIGGERED
---------------------------------
- Edge triggered is like a one-shot notification, read(s) may be blocked indefinitely. Better to use non-blocking fds w/ edge triggered events
- Level triggered events will always notify user if an fd is ready until it is handled. Better to use this since I don't want to deal w/ non-blocking

- Or is it?? I need to do more research on this

# 10-21-24

Finished up Signal class. Signal flag will hold custom handlers for sigterm and sigint signals because I don't want components to shutdown anything. Only component responsible for handiling shutdown is Signal. Vehicle manager will read status code of Signal class to determine shutdown.






