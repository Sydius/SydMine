#include "client.hpp"

#include "logging.hpp"

Client::~Client()
{
    LOG_DEBUG << "client destroyed (EID " << m_eid << ")\n";
}

Client::Client(boost::asio::io_service & ioService)
    : m_socket(ioService)
    , m_eid(0)
{
    LOG_DEBUG << "client created\n";
}
