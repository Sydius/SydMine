#pragma once

#include <boost/asio.hpp>

class Server
{
    public:
        Server(boost::asio::io_service &ioService);

        bool tick(void);
};
