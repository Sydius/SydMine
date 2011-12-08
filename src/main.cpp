#include <chrono>
#include <iostream>
#include "server.hpp"

int main(int argc, char * argv[])
{
    boost::asio::io_service ioService;
    Server server(ioService);

    auto time = std::chrono::monotonic_clock::now();
    bool keepGoing = true;
    while (keepGoing) {
        static const auto duration = std::chrono::milliseconds(50);
        while (std::chrono::monotonic_clock::now() - time > duration) {
            time += duration;

            keepGoing = server.tick();
            if (!keepGoing) break;
        }
        ioService.poll();
    }

    return 0;
}
