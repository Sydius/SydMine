#pragma once

#include <boost/asio.hpp>

class Server
{
    public:
        Server(boost::asio::io_service &ioService, int port);

        bool tick(void);
};
