#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include "client.hpp"
#include "eid.hpp"
#include "chunkmanager.hpp"

class Server
{
    public:
        typedef std::unordered_map<EID, Client::pointer> ClientList;

        Server(boost::asio::io_service & ioService, int port, const std::string & configFile);

        bool tick(void);

        int getPlayingCount(void) const;
        int getPlayingMax(void) const;
        std::string getDescription(void) const;
        bool authRequired(void) const;
        unsigned int getTicks(void) const;

        void reloadConfig(void);

        void chunkSubscribe(Client * client, int x, int z);

    private:
        void accept(void);
        void handleAccept(Client::pointer newClient,
                const boost::system::error_code & error);
        void checkClientStatus(void);

        boost::asio::ip::tcp::acceptor m_acceptor;
        ClientList m_clients;
        std::string m_configFile;

        ChunkManager m_chunkManager;

        unsigned int m_curTick;

        // Config file options
        int m_maxPlayers;
        std::string m_desc;
        bool m_requireAuth;
};
