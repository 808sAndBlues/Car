#include "Signal.h"

void Signal::init()
{
    set_signal_masks();
    set_signal_handler();
}

void Signal::set_signal_masks()
{
    sigset_t signal_set;

    if (sigemptyset(&signal_set) == -1) {
        std::cout << "Error filling w/ empty set\n";
        std::perror("sigemptyset");
        std::exit(-1);
    }

    if (sigaddset(&signal_set, SIGTERM) == -1) {
        std::cout << "Failed to add SIGTERM to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigaddset(&signal_set, SIGINT) == -1) {
        std::cout << "Failed to add SIGINT to signal set\n";
        std::perror("sigaddset");
        std::exit(-1);
    }

    if (sigprocmask(SIG_UNBLOCK, &signal_set, 0) == -1) {
        std::cout << "Failed to unblock signal set\n";
        std::perror("sigprocmask");
        std::exit(-1);
    }

    // TODO: Update this w/ Log statements
    std::cout << "Ready to catch SIGTERM and SIGINT\n";
    
}

void Signal::set_signal_handler()
{
    struct sigaction act = {0};
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &signal_handler;
      
    if (sigaction(SIGINT, &act, nullptr) == -1) {
        std::cout << "Failed to register handler for SIGINT\n";
        std::perror("sigaction");
        std::exit(-1);
    }

    if (sigaction(SIGTERM, &act, nullptr) == -1) {
        std::cout << "Failed to register handler for SIGTERM\n";
        std::perror("sigaction");
        std::exit(-1);
    }
}

void signal_handler(int signum, siginfo_t* info, void* context)
{
    _kill_flag.kill();
}

