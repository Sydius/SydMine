#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <fstream>
#include "server.hpp"
#include "logging.hpp"

Server::Server(boost::asio::io_service & ioService, int port, const std::string & configFile)
    : m_acceptor(ioService, boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), port))
    , m_clients()
    , m_configFile(configFile)
    , m_chunkManager(".") // TODO: replace with real world dir
    , m_curTick(0)
    , m_maxPlayers(0)
    , m_desc()
    , m_requireAuth(true)
{
    LOG_NOTICE << "accepting connections at " << m_acceptor.local_endpoint() << "\n";
    reloadConfig();
    accept();
}

void Server::checkClientStatus(void)
{
    // Check for disconnected clients
    for (auto & client: m_clients) {
        if (client.second->getState() == Client::DISCONNECTED) {
            LOG_NOTICE << "client disconnected (EID " << client.second->getEID() << "): " << client.second->socket().remote_endpoint() << "\n";
            m_clients.erase(client.first);
        } else if (client.second->getState() == Client::PLAYING) {
            if (!(m_curTick % 20)) {
                client.second->sendTimeUpdate(m_curTick);
            }
        }
    }
}

bool Server::tick(void)
{
    checkClientStatus();

    m_curTick++;
    return true;
}

int Server::getPlayingCount(void) const
{
    int playing = 0;
    for (auto & client: m_clients) {
        if (client.second->getState() == Client::PLAYING) {
            playing++;
        }
    }
    return playing;
}

int Server::getPlayingMax(void) const
{
    return m_maxPlayers;
}

std::string Server::getDescription(void) const
{
    return m_desc;
}

bool Server::authRequired(void) const
{
    return m_requireAuth;
}

unsigned int Server::getTicks(void) const
{
    return m_curTick;
}

void Server::reloadConfig(void)
{
    namespace po = boost::program_options;
    po::options_description desc("Config Options");
    desc.add_options()
        ("desc", po::value<std::string>(&m_desc)->default_value("SydMine"), "server description")
        ("maxPlayers", po::value<int>(&m_maxPlayers)->default_value(20), "maximum number of players")
        ("requireAuth", po::value<bool>(&m_requireAuth)->default_value(true), "whether or not auth is required")
    ;

    po::variables_map vm;
    std::ifstream in(m_configFile); // bizarre, but the config file parser won't take a string
    po::store(po::parse_config_file(in, desc), vm);
    po::notify(vm);
}

void Server::chunkSubscribe(Client * client, int x, int z)
{
    m_chunkManager.subscribe(client, x, z);
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
