#include <boost/asio.hpp>

int main(int argc, char * argv[])
{
    boost::asio::io_service ioService;

    while (true) {
        ioService.poll();
    }

    return 0;
}
