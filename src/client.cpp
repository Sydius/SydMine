#include "client.hpp"

#include <istream>
#include <ostream>
#include <utf8.h>
#include <locale>
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

Client::Client(boost::asio::io_service & ioService, Server * server)
    : m_socket(ioService)
    , m_eid(0)
    , m_state(LISTENING)
    , m_incoming()
    , m_outgoing()
    , m_readNeeded(1)
    , m_data()
    , m_dataItem(0)
    , m_writing(false)
    , m_server(server)
{
    LOG_DEBUG << "client created\n";
}

void Client::writeIfNeeded(void)
{
    if (m_writing || !m_outgoing.size()) {
        return;
    }

    m_writing = true;

    boost::asio::async_write(m_socket, m_outgoing,
            std::bind(&Client::handleWrite, this, std::placeholders::_1));
}

void Client::sendKick(const std::string & reason)
{
    set(mcCommandType(0xFF));
    setString(reason);
}

void Client::handleWrite(const boost::system::error_code & error)
{
    if (error) {
        m_state = DISCONNECTED;
        return;
    }

    m_writing = false;
    writeIfNeeded();
}

void Client::handleRead(const boost::system::error_code & error)
{
    if (error) {
        m_state = DISCONNECTED;
        return;
    }
    LOG_DEBUG << "read done\n";

    m_readNeeded = 0;
    m_dataItem = 0;

    mcCommandType command;
    if (!get(command)) return read();

    LOG_DEBUG << "got command: " << unsigned(command) << "\n";
    switch (command) {
        case 0xFE: // Server list ping
            sendKick(u8"SydMine test server\xc2\xa7""0\xc2\xa7""42");
            break;
        default:
            sendKick(u8"lol, u mad?");
            break;
    }

    m_readNeeded = 1;
    m_data.clear();
    if (m_incoming.size()) {
        handleRead(error);
    } else {
        read();
    }

    writeIfNeeded();
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

bool Client::get(mcShort & s)
{
    return ntohs(getHelper(s, m_data, m_dataItem, m_incoming, m_readNeeded));
}

bool Client::get(mcInt & i)
{
    return ntohl(getHelper(i, m_data, m_dataItem, m_incoming, m_readNeeded));
}

bool Client::get(mcLong & l)
{
    return ntohll(getHelper(l, m_data, m_dataItem, m_incoming, m_readNeeded));
}

bool Client::get(mcFloat & f)
{
    return ntohl(getHelper(f, m_data, m_dataItem, m_incoming, m_readNeeded));
}

bool Client::get(mcDouble & d)
{
    return ntohll(getHelper(d, m_data, m_dataItem, m_incoming, m_readNeeded));
}

bool Client::get(mcCommandType & c)
{
    return getHelper(c, m_data, m_dataItem, m_incoming, m_readNeeded);
}

// Template helper
template<class T>
void setHelper(T x, boost::asio::streambuf & m_outgoing)
{
    std::ostream outputStream(&m_outgoing);
    outputStream.write(reinterpret_cast<char *>(&x), sizeof(T));
}

void Client::set(mcByte b)
{
    setHelper(b, m_outgoing);
}

void Client::set(mcShort s)
{
    setHelper(htons(s), m_outgoing);
}

void Client::set(mcInt i)
{
    setHelper(htonl(i), m_outgoing);
}

void Client::set(mcLong l)
{
    setHelper(htonll(l), m_outgoing);
}

void Client::set(mcFloat f)
{
    setHelper(htonl(f), m_outgoing);
}

void Client::set(mcDouble d)
{
    setHelper(htonll(d), m_outgoing);
}

void Client::set(mcCommandType c)
{
    setHelper(c, m_outgoing);
}

void Client::setString(const std::string & str)
{
    std::u16string utf16;
    
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16));
    set(mcShort(utf16.length()));

    for (mcShort c: utf16) {
        set(c);
    }
}
