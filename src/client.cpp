#include "client.hpp"

Client::Client(boost::asio::io_service & ioService)
    : m_socket(ioService)
{
}
