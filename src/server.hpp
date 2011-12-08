#pragma once

#include <boost/asio.hpp>
#include "client.hpp"

class Server
{
    public:
        Server(boost::asio::io_service & ioService, int port);

        bool tick(void);

    private:
        void accept(void);

        boost::asio::ip::tcp::acceptor m_acceptor;
};
