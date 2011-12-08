#include "server.hpp"

Server::Server(boost::asio::io_service & ioService, int port)
    : m_ioService(ioService)
{

}

bool Server::tick(void)
{
    return true;
}
