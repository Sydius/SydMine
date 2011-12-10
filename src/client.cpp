#include "client.hpp"

#include <istream>
#include <ostream>
#include <utf8.h>
#include <locale>
#include <boost/lexical_cast.hpp>
#include "logging.hpp"
#include "server.hpp"

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
    , m_readNeeded(sizeof(mcCommandType))
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
    set(reason);
}

void Client::disconnect(const std::string & reason)
{
    sendKick(reason);
    m_state = DISCONNECTING;
}

void Client::handleWrite(const boost::system::error_code & error)
{
    if (error) {
        m_state = DISCONNECTED;
        return;
    }

    m_writing = false;

    if (m_state == DISCONNECTING) {
        m_state = DISCONNECTED;
        m_socket.cancel();
    } else {
        writeIfNeeded();
    }
}

void Client::handleRead(const boost::system::error_code & error)
{
    if (error) {
        m_state = DISCONNECTED;
        return;
    }

    m_readNeeded = 0;
    m_dataItem = 0;

    mcCommandType command;
    if (!get(command)) return read();

    LOG_DEBUG << "got command: " << unsigned(command) << "\n";

    if (m_state == CONNECTED) {
        switch (command) {
            case 0x02: // Handshake
                handleHandshake();
                break;
            case 0xFE: // Server list ping
                handlePing();
                break;
            default:
                disconnect(u8"Packet stream corrupt");
                break;
        }
    }

    m_readNeeded = sizeof(mcCommandType);
    m_data.clear();
    if (m_incoming.size()) {
        handleRead(error);
    } else {
        read();
    }

    writeIfNeeded();
}

void Client::handleHandshake(void)
{
    std::string username;
    if (!get(username)) return read();

    set(mcCommandType(0x02));
    if (m_server->authRequired()) {
    } else {
        set(u8"-");
    }
}

void Client::handlePing(void)
{
    static const std::string delim(u8"\xc2\xa7");
    std::string reason = m_server->getDescription() + delim;
    reason += boost::lexical_cast<std::string>(m_server->getPlayingCount()) + delim;
    reason += boost::lexical_cast<std::string>(m_server->getPlayingMax());
    disconnect(reason);
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
    bool ret = getHelper(s, m_data, m_dataItem, m_incoming, m_readNeeded);
    s = ntohs(s);
    return ret;
}

bool Client::get(mcInt & i)
{
    bool ret = getHelper(i, m_data, m_dataItem, m_incoming, m_readNeeded);
    i = ntohl(i);
    return ret;
}

bool Client::get(mcLong & l)
{
    bool ret = getHelper(l, m_data, m_dataItem, m_incoming, m_readNeeded);
    l = ntohll(l);
    return ret;
}

bool Client::get(mcFloat & f)
{
    bool ret = getHelper(f, m_data, m_dataItem, m_incoming, m_readNeeded);
    f = ntohl(f);
    return ret;
}

bool Client::get(mcDouble & d)
{
    bool ret = getHelper(d, m_data, m_dataItem, m_incoming, m_readNeeded);
    d = ntohll(d);
    return ret;
}

bool Client::get(mcCommandType & c)
{
    return getHelper(c, m_data, m_dataItem, m_incoming, m_readNeeded);
}

bool Client::get(std::string & str)
{
    mcShort len = 0;
    if (!get(len)) return false;

    std::u16string utf16;
    for (mcShort i = 0; i < len; i++) {
        mcShort c;
        if (!get(c)) return false;
        utf16 += c;
    }

    utf8::utf16to8(utf16.begin(), utf16.end(), std::back_inserter(str));
    return true;
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

void Client::set(const std::string & str)
{
    std::u16string utf16;
    
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16));
    set(mcShort(utf16.length()));

    for (mcShort c: utf16) {
        set(c);
    }
}
