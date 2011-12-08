#include <boost/asio.hpp>
#include <chrono>
#include <iostream>

int main(int argc, char * argv[])
{
    boost::asio::io_service ioService;

    auto time = std::chrono::monotonic_clock::now();
    while (true) {
        static const auto duration = std::chrono::milliseconds(50);
        if (std::chrono::monotonic_clock::now() - time > duration) {
            time += duration;

            std::cout << "tick\n";
        }
        ioService.poll();
    }

    return 0;
}
