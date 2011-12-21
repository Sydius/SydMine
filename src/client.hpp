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

#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <vector>
#include <utility>
#include "eid.hpp"
#include "types.hpp"
#include "netutil.hpp"
#include "chunk.hpp"
#include "entity.hpp"

class Server;

class Client: public Entity
{
    public:
        typedef std::shared_ptr<Client> pointer;
        typedef enum {
            LISTENING,
            CONNECTED,
            PLAYING,
            DISCONNECTING,
            DISCONNECTED
        } State;

        static pointer create(boost::asio::io_service & ioService, Server * server)
        {
            return pointer(new Client(ioService, server));
        }

        boost::asio::ip::tcp::socket & socket(void)
        {
            return m_socket;
        }

        std::string getName(void) const
        {
            return m_username;
        }

        void read(void);

        State getState(void) const
        {
            return m_state;
        }

        bool hasPlayed(void) const
        {
            return m_hasPlayed;
        }

        bool waitingForChunks(void) const
        {
            return m_state == PLAYING && !m_chunksLoaded.size();
        }

        void writeIfNeeded(void);

        void sendKick(const std::string & reason);
        void sendInitChunk(Chunk::Coord x, Chunk::Coord z, bool enable);
        void sendChunk(Chunk::Coord x, Chunk::Coord z, const Chunk & chunk);
        void sendTimeUpdate(unsigned int time);
        void sendChat(const std::string & msg);
        void sendEntityMove(const Entity * entity);
        void sendPlayerPosition(void);

        void addPeer(Client * peer);
        void removePeer(Client * peer);

        void updateChunk(const Chunk & chunk, Chunk::Coord chunkX, Chunk::Coord chunkZ);
        void updateBlock(int x, int y, int z, int type);

        void disconnect(const std::string & reason);

        ~Client();

        Client & operator=(const Client &) = delete;
        Client(const Client &) = delete;
        Client() = default;

    private:
        Client(boost::asio::io_service & ioService, Server * server);

        void handleWrite(const boost::system::error_code & error);
        void handleRead(const boost::system::error_code & error);

        void handleLogin(void);
        void handleHandshake(void);
        void handlePing(void);

        void handleKeepAlive(void);
        void handleChat(void);
        void handleOnGround(void);
        void handlePlayerPositionUpdate(void);
        void handlePlayerLook(void);
        void handlePlayerPosition(void);
        void handleDig(void);
        void handleHoldingChange(void);
        void handleAnimation(void);
        void handlePlayerAction(void);
        void handleCloseWindow(void);

        bool get(mcByte & b);
        bool get(mcUByte & b);
        bool get(mcShort & s);
        bool get(mcUShort & s);
        bool get(mcInt & i);
        bool get(mcUInt & i);
        bool get(mcLong & l);
        bool get(mcULong & l);
        bool get(mcFloat & f);
        bool get(mcDouble & d);
        bool get(std::string & str);

        void set(mcByte b);
        void set(mcUByte b);
        void set(mcShort s);
        void set(mcUShort s);
        void set(mcInt i);
        void set(mcUInt i);
        void set(mcLong l);
        void set(mcULong l);
        void set(mcFloat f);
        void set(mcDouble d);
        void set(const std::string & str);

        boost::asio::ip::tcp::socket m_socket;
        State m_state;
        bool m_hasPlayed;
        boost::asio::streambuf m_incoming;
        boost::asio::streambuf m_outgoing;
        std::size_t m_readNeeded;

        typedef std::vector<mcLargest> DataList;
        DataList m_data;
        unsigned int m_dataItem;

        bool m_writing;
        Server * m_server;

        std::string m_username;

        typedef std::pair<Chunk::Coord, Chunk::Coord> ChunkCoord;
        typedef std::vector<ChunkCoord> ChunkCoordList;
        ChunkCoordList m_chunksLoaded;
};
