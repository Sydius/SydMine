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
}

Client::Client(boost::asio::io_service & ioService)
    : m_socket(ioService)
    , m_eid(0)
    , m_state(LISTENING)
{
    LOG_DEBUG << "client created\n";
}
