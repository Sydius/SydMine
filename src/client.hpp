#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "eid.hpp"

class Client
{
    public:
        typedef std::shared_ptr<Client> pointer;
        typedef enum {
            LISTENING,
            CONNECTED,
            DISCONNECTED
        } State;

        static pointer create(boost::asio::io_service & ioService)
        {
            return pointer(new Client(ioService));
        }

        boost::asio::ip::tcp::socket & socket(void)
        {
            return m_socket;
        }

        void setEID(EID eid)
        {
            m_eid = eid;
        }

        void read(void);

        ~Client();

    private:
        Client(boost::asio::io_service & ioService);

        boost::asio::ip::tcp::socket m_socket;
        EID m_eid;
        State m_state;
};
