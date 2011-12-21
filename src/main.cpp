/*  Coypright (C) 2011 Christopher Allen Ogden
 *
 *     This file is part of SydMine.
 *
 *  SydMine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SydMine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SydMine.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    int port = 0;
    std::string config;
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config", po::value<std::string>(&config)->default_value("sydmine.cfg"), "config file location")
        ("port", po::value<int>(&port)->default_value(25565), "port to listen on")
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

    try {
        boost::asio::io_service ioService;
        Server server(ioService, port, config);

        while (keepGoing) {
            static const auto duration = std::chrono::milliseconds(TICK_DURATION);
            auto nextFrame = std::chrono::monotonic_clock::now() + duration;

            ioService.poll();
            keepGoing = server.tick();

            std::this_thread::sleep_until(nextFrame);
        }
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
