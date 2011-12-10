#include "server.hpp"
#include "logging.hpp"

Server::Server(boost::asio::io_service & ioService, int port)
    : m_acceptor(ioService, boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), port))
    , m_clients()
{
    LOG_NOTICE << "accepting connections at " << m_acceptor.local_endpoint() << "\n";
    accept();
}

void Server::checkClientStatus(void)
{
    // Check for disconnected clients
    for (auto & client: m_clients) {
        if (client.second->getState() == Client::DISCONNECTED) {
            LOG_NOTICE << "client disconnected (EID " << client.second->getEID() << "): " << client.second->socket().remote_endpoint() << "\n";
            m_clients.erase(client.first);
        }
    }
}

bool Server::tick(void)
{
    checkClientStatus();
    return true;
}

void Server::accept(void)
{
    Client::pointer newClient = Client::create(m_acceptor.io_service(), this);

    m_acceptor.async_accept(newClient->socket(),
            std::bind(&Server::handleAccept, this, newClient, std::placeholders::_1));
}

void Server::handleAccept(Client::pointer newClient,
        const boost::system::error_code & error)
{
    if (!error) {
        EID clientEID = generateNewEID();
        m_clients[clientEID] = newClient;
        newClient->setEID(clientEID);

        LOG_NOTICE << "client connected (EID " << clientEID << "): " << newClient->socket().remote_endpoint() << "\n";
        newClient->read();

        accept();
    } else {
        // TODO: Log
    }
}
