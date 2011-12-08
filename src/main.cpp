#ifdef __linux
#include <unistd.h> // for daemon
#include <signal.h> // for sigterm
#endif

#include <boost/program_options.hpp>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
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
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
#ifdef __linux
        ("daemonize", "daemonize process")
#endif
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

#ifdef __linux
    if (vm.count("daemonize"))
    {
        if (!daemonize()) return EXIT_FAILURE;
    }
#endif

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
