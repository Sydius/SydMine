#include "client.hpp"

#include "logging.hpp"

Client::~Client()
{
    LOG_DEBUG << "client destroyed (EID " << m_eid << ")\n";
}

void Client::read(void)
{
    if (m_state == LISTENING)
        m_state = CONNECTED;

    boost::asio::async_read(m_socket, m_incoming,
            std::bind(&Client::isReadDone, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&Client::handleRead, this, std::placeholders::_1, std::placeholders::_2));
}

Client::Client(boost::asio::io_service & ioService)
    : m_socket(ioService)
    , m_eid(0)
    , m_state(LISTENING)
    , m_incoming()
    , m_outgoing()
{
    LOG_DEBUG << "client created\n";
}

bool Client::isReadDone(const boost::system::error_code & error, std::size_t bytes_transferred)
{
    return true;
}

void Client::handleRead(const boost::system::error_code & error, std::size_t bytes_transferred)
{
    LOG_DEBUG << "read done\n";
}
