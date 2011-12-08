#include "server.hpp"

Server::Server(boost::asio::io_service & ioService, int port)
    : m_acceptor(ioService, boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), port))
{
    accept();
}

bool Server::tick(void)
{
    return true;
}

void Server::accept(void)
{
    Client::pointer newClient = Client::create(m_acceptor.io_service());
}
