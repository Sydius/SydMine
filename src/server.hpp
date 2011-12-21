/*  Coypright (C) 2011 Christopher Allen Ogden
 *
 *     This file is part of SydMine.
 *
 *  SydMine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SydMine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SydMine.  If not, see <http://www.gnu.org/licenses/>.
 */

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

        int getChunkRange(void) const;
        int getPlayingCount(void) const;
        int getPlayingMax(void) const;
        std::string getDescription(void) const;
        bool authRequired(void) const;
        unsigned int getTicks(void) const;
        
        void digBlock(int x, int y, int z);

        void notifyChat(Client * client, const std::string & msg);
        void notifyConnected(Client * client, bool connected);

        void reloadConfig(void);

    private:
        std::string getChunkKey(int world, Chunk::Coord x, Chunk::Coord z);
        void accept(void);
        void handleAccept(Client::pointer newClient,
                const boost::system::error_code & error);
        void checkClientStatus(void);
        void sendUpdatedPositions(Client * client);
        Chunk & getChunk(int world, Chunk::Coord x, Chunk::Coord z);

        boost::asio::ip::tcp::acceptor m_acceptor;
        ClientList m_clients;
        std::string m_configFile;

        unsigned int m_curTick;

        typedef std::unordered_map<std::string, Chunk> ChunkList;
        ChunkList m_loadedChunks;

        // Config file options
        int m_maxPlayers;
        std::string m_desc;
        bool m_requireAuth;
};
