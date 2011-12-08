#ifdef __linux
#include <unistd.h> // for daemon
#include <signal.h> // for sigterm
#endif

#include <chrono>
#include <thread>
#include <cstdlib>
#include "server.hpp"

#define TICK_DURATION 50

// When false, stop the main loop
bool keepGoing = true;

#ifdef __linux
void term(int signum)
{
    keepGoing = false;
}
#endif

bool daemonize(void)
{
#ifdef __linux
    if (!daemon(0,0)) {
        signal(SIGTERM, term);
        return true;
    }
    else return false;
#endif
    return true;
}

int main(int argc, char * argv[])
{
    if (!daemonize()) return EXIT_FAILURE;

    boost::asio::io_service ioService;
    Server server(ioService);

    auto time = std::chrono::monotonic_clock::now();
    while (keepGoing) {
        static const auto duration = std::chrono::milliseconds(TICK_DURATION);
        auto nextFrame = time + duration;
        if (std::chrono::monotonic_clock::now() > nextFrame) {
            time += duration;

            keepGoing = server.tick();
            if (!keepGoing) break;
        }
        ioService.poll();

        std::this_thread::sleep_until(nextFrame);
    }

    return EXIT_SUCCESS;
}
