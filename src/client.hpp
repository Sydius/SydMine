#pragma once

#include <memory>
#include <boost/asio.hpp>

class Client
{
    public:
        typedef std::shared_ptr<Client> pointer;

        static pointer create(boost::asio::io_service & ioService)
        {
            return pointer(new Client(ioService));
        }

        Client(boost::asio::io_service & ioService);
};
