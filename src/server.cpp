#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "server.hpp"
#include "logging.hpp"
#include "chunk.hpp"

Server::Server(boost::asio::io_service & ioService, int port, const std::string & configFile)
    : m_acceptor(ioService, boost::asio::ip::tcp::endpoint(
                boost::asio::ip::tcp::v4(), port))
    , m_clients()
    , m_configFile(configFile)
    , m_curTick(0)
    , m_loadedChunks()
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
    auto client = m_clients.begin();
    while (client != m_clients.end()) {
        if ((*client).second->getState() == Client::DISCONNECTED) {
            if ((*client).second->hasPlayed()) {
                notifyConnected((*client).second.get(), false);
            }
            LOG_NOTICE << "client disconnected (EID " << (*client).second->getEID() << "): " << (*client).second->socket().remote_endpoint() << "\n";
            client = m_clients.erase(client);
            continue;
        } else if ((*client).second->getState() == Client::PLAYING) {
            if (!(m_curTick % 20)) {
                (*client).second->sendTimeUpdate(m_curTick);
            }

            sendUpdatedPositions((*client).second.get());
        }
        ++client;
    }

    for (auto & client: m_clients) {
        if (client.second->getState() == Client::PLAYING) {
            client.second->tickPosition();
        }
    }
}

void Server::sendUpdatedPositions(Client * client)
{
    for (auto & peer: m_clients) {
        if (peer.second->getState() == Client::PLAYING && client->getEID() != peer.second->getEID()) {
            client->sendEntityMove(peer.second.get());
        }
    }

    bool waitingForChunks = client->waitingForChunks();

    // Ensure the proper chunks are loaded
    Chunk::Coord chunkX = client->getChunkX();
    Chunk::Coord chunkZ = client->getChunkZ();

    static const Chunk::Coord chunkRange = getChunkRange() - 1;
    for (Chunk::Coord cx = chunkX - chunkRange; cx <= chunkX + chunkRange; cx++) {
        for (Chunk::Coord cz = chunkZ - chunkRange; cz <= chunkZ + chunkRange; cz++) {
            client->updateChunk(getChunk(0, cx, cz), cx, cz); // TODO: replace world value
        }
    }

    if (waitingForChunks && !client->waitingForChunks()) {
        client->sendPlayerPosition();
    }
}

Chunk & Server::getChunk(int world, Chunk::Coord x, Chunk::Coord z)
{
    std::string key = getChunkKey(world, x, z);

    auto chunkIter = m_loadedChunks.find(key);
    if (chunkIter != m_loadedChunks.end()) {
        return (*chunkIter).second;
    }

    m_loadedChunks[key] = Chunk();

    return m_loadedChunks[key];
}

bool Server::tick(void)
{
    checkClientStatus();

    auto chunk = m_loadedChunks.begin();
    
    while (chunk != m_loadedChunks.end()) {
        std::vector<std::string> parts;
        boost::split(parts, (*chunk).first, boost::is_any_of(":|"));
        //int world = boost::lexical_cast<int>(parts[0]);
        Chunk::Coord x = boost::lexical_cast<Chunk::Coord>(parts[1]);
        Chunk::Coord z = boost::lexical_cast<Chunk::Coord>(parts[2]);

        bool used = false;
        for (auto & client: m_clients) {
            if (client.second->getState() == Client::PLAYING) {
                if (abs(client.second->getChunkX() - x) < getChunkRange() &&
                    abs(client.second->getChunkZ() - z) < getChunkRange()) {
                    used = true;
                }
            }
        }

        if (!used) {
            chunk = m_loadedChunks.erase(chunk);
        } else {
            ++chunk;
        }
    }

    m_curTick++;
    return true;
}

int Server::getChunkRange(void) const
{
    return 9; // TODO: config
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

void Server::digBlock(int x, int y, int z)
{
    Chunk::Coord chunkX = floor(double(x) / 16);
    Chunk::Coord chunkZ = floor(double(z) / 16);
    Chunk & chunk = getChunk(1, chunkX, chunkZ); // TODO: replace world value
    
    for (auto & client: m_clients) {
        if (client.second->getState() == Client::PLAYING) {
            if (abs(client.second->getChunkX() - chunkX) < getChunkRange() &&
                abs(client.second->getChunkZ() - chunkZ) < getChunkRange()) {
                client.second->updateBlock(x, y, z, 0);
            }
        }
    }

    x = x%16;
    z = z%16;
    x = x < 0 ? 16 + x : x;
    z = z < 0 ? 16 + z : z;
    chunk.setBlockType(x, y, z, 0);
}

void Server::notifyChat(Client * client, const std::string & msg)
{
    // TODO: sanitize, add support for commands
    std::string chat = std::string("<") + client->getName() + "> " + msg;

    for (auto & peer: m_clients) {
        if (peer.second->getState() == Client::PLAYING) {
            peer.second->sendChat(chat);
        }
    }
}

void Server::notifyConnected(Client * client, bool connected)
{
    // TODO: sanitize, add support for commands
    std::string chat = std::string("") + client->getName() + (connected ? u8" connected" : u8" disconnected");

    for (auto & peer: m_clients) {
        if (peer.second->getState() == Client::PLAYING) {
            peer.second->sendChat(chat);
            if (peer.second->getEID() != client->getEID()) {
                if (connected) {
                    peer.second->addPeer(client);
                    client->addPeer(peer.second.get());
                } else {
                    peer.second->removePeer(client);
                }
            }
        }
    }
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

std::string Server::getChunkKey(int world, Chunk::Coord x, Chunk::Coord z)
{
    return boost::lexical_cast<std::string>(world) + ":" +
        boost::lexical_cast<std::string>(x) + "|" +
        boost::lexical_cast<std::string>(z);
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
