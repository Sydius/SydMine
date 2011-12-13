#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include "client.hpp"
#include "eid.hpp"

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

        void notifyChat(Client * client, const std::string & msg);
        void notifyConnected(Client * client, bool connected);

        void reloadConfig(void);

    private:
        void accept(void);
        void handleAccept(Client::pointer newClient,
                const boost::system::error_code & error);
        void checkClientStatus(void);
        void sendUpdatedPositions(Client * client);

        boost::asio::ip::tcp::acceptor m_acceptor;
        ClientList m_clients;
        std::string m_configFile;

        unsigned int m_curTick;

        // Config file options
        int m_maxPlayers;
        std::string m_desc;
        bool m_requireAuth;
};
