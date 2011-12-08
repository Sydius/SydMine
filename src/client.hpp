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

        boost::asio::ip::tcp::socket & socket(void)
        {
            return m_socket;
        }

    private:
        Client(boost::asio::io_service & ioService);

        boost::asio::ip::tcp::socket m_socket;
};
