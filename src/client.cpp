#include "client.hpp"

#include <istream>
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
            boost::asio::transfer_at_least(m_readNeeded),
            std::bind(&Client::handleRead, this, std::placeholders::_1));
}

Client::Client(boost::asio::io_service & ioService)
    : m_socket(ioService)
    , m_eid(0)
    , m_state(LISTENING)
    , m_incoming()
    , m_outgoing()
    , m_readNeeded(1)
    , m_data()
    , m_dataItem(0)
{
    LOG_DEBUG << "client created\n";
}

void Client::handleRead(const boost::system::error_code & error)
{
    LOG_DEBUG << "read done\n";

    m_readNeeded = 0;
    m_dataItem = 0;

    mcCommandType command;
    if (!get(command)) return read();

    m_readNeeded = 1;
    m_data.clear();
    read();
}

// Template helper
template<class T>
bool getHelper(T & x, Client::DataList & m_data, unsigned int & m_dataItem, boost::asio::streambuf & m_incoming, std::size_t & m_readNeeded)
{
    if (m_dataItem < m_data.size()) { // We've read it before
        x = m_data[m_dataItem];
    } else if (m_incoming.size() >= sizeof(T)) { // We'll read it now
        std::istream inputStream(&m_incoming);
        inputStream.read(reinterpret_cast<char *>(&x), sizeof(T));
        m_data.push_back(x);
    } else { // We can't read it yet
        m_readNeeded += sizeof(T) - m_incoming.size();
        return false;
    }
    m_dataItem++;
    return true;
}

bool Client::get(mcByte & b)
{
    return getHelper(b, m_data, m_dataItem, m_incoming, m_readNeeded);
}
