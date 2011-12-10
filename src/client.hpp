#pragma once

#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "eid.hpp"
#include "types.hpp"
#include "netutil.hpp"

class Server;

class Client
{
    public:
        typedef std::shared_ptr<Client> pointer;
        typedef enum {
            LISTENING,
            CONNECTED,
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

        void setEID(EID eid)
        {
            m_eid = eid;
        }

        EID getEID(void) const
        {
            return m_eid;
        }

        void read(void);

        State getState(void) const
        {
            return m_state;
        }

        void writeIfNeeded(void);

        void sendKick(const std::string & reason);

        ~Client();

        Client & operator=(const Client &) = delete;
        Client(const Client &) = delete;
        Client() = default;

    private:
        Client(boost::asio::io_service & ioService, Server * server);

        void handleWrite(const boost::system::error_code & error);
        void handleRead(const boost::system::error_code & error);

        bool get(mcByte & b);
        bool get(mcShort & s);
        bool get(mcInt & i);
        bool get(mcLong & l);
        bool get(mcFloat & f);
        bool get(mcDouble & d);
        bool get(mcCommandType & c);

        void set(mcByte b);
        void set(mcShort s);
        void set(mcInt i);
        void set(mcLong l);
        void set(mcFloat f);
        void set(mcDouble d);
        void set(mcCommandType c);
        void setString(const std::string & str);

        boost::asio::ip::tcp::socket m_socket;
        EID m_eid;
        State m_state;
        boost::asio::streambuf m_incoming;
        boost::asio::streambuf m_outgoing;
        std::size_t m_readNeeded;

        typedef std::vector<mcLargest> DataList;
        DataList m_data;
        unsigned int m_dataItem;

        bool m_writing;
        Server * m_server;
};
