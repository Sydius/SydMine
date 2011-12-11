#include "client.hpp"

#include <istream>
#include <ostream>
#include <sstream>
#include <utf8.h>
#include <locale>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include "logging.hpp"
#include "server.hpp"
#include "chunkmanager.hpp"

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
    , m_username("Unknown")
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

void Client::sendInitChunk(int x, int z)
{
    set(mcCommandType(0x32));
    set(mcInt(x));
    set(mcInt(z));
    set(mcByte(1)); // enable
}

void Client::sendChunk(int x, int z, const Chunk & chunk)
{
    set(mcCommandType(0x33));
    set(mcInt(x*chunk.getXSize()));
    set(mcShort(0)); // y
    set(mcInt(z*chunk.getZSize()));
    set(mcByte(chunk.getXSize()-1));
    set(mcByte(chunk.getYSize()-1));
    set(mcByte(chunk.getZSize()-1));

    std::string uncompressed((const char *)chunk.getData(), chunk.getDataSize());

    std::string compressed;
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(std::back_inserter(compressed));
    boost::iostreams::copy(boost::make_iterator_range(uncompressed), out);

    set(mcInt(compressed.length()));

    std::ostream outputStream(&m_outgoing);
    outputStream.write(reinterpret_cast<const char *>(compressed.c_str()), compressed.length());
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

    LOG_DEBUG << "got command: " << std::hex << unsigned(command) << "\n";

    if (m_state == CONNECTED) {
        switch (command) {
            case 0x01: // Login request
                handleLogin();
                break;
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
    } else if (m_state == PLAYING) {
        switch(command) {
            case 0x00: // Keep-alive
                handleKeepAlive();
                break;
            case 0x0A: // On-ground
                handleOnGround();
                break;
            case 0x0B: // Player position update
                handlePlayerPositionUpdate();
                break;
            case 0x0C: // Player look
                handlePlayerLook();
                break;
            case 0x0D: // Player position
                handlePlayerPosition();
                break;
            default:
                disconnect(u8"Packet stream corrupt");
                break;
        }
    }

    m_readNeeded = sizeof(mcCommandType);
    m_data.clear();

    writeIfNeeded();
    if (m_state == DISCONNECTING) return;

    if (m_incoming.size()) {
        handleRead(error);
    } else {
        read();
    }
}

void Client::handleLogin(void)
{
    mcInt protocolVersion;
    if (!get(protocolVersion)) return read();

    if (protocolVersion != 22) return disconnect(u8"Unsupported protocol version");

    if (!get(m_username)) return read();
    mcLong unusedL;
    if (!get(unusedL)) return read();
    mcInt unusedI;
    if (!get(unusedI)) return read();
    mcByte unusedB;
    if (!get(unusedB)) return read();
    if (!get(unusedB)) return read();
    if (!get(unusedB)) return read();
    if (!get(unusedB)) return read();

    // TODO: support authentication
    
    // TODO: add validation
    
    if (m_server->getPlayingCount() >= m_server->getPlayingMax())
        return disconnect(u8"Server full");

    set(mcCommandType(0x01));
    set(m_eid);
    set(u8""); // Unused string
    set(mcLong(1)); // TODO: fill with map seed
    set(mcInt(0)); // survival mode
    set(mcByte(0)); // overworld
    set(mcByte(3)); // difficulty TODO: add server config
    set(mcUByte(128)); // world height
    set(mcUByte(m_server->getPlayingCount()));

    m_state = PLAYING;
    m_server->chunkSubscribe(this, 0, 0); // TODO: use player coords
    m_server->chunkSubscribe(this, -1, 0); // TODO: use player coords
    m_server->chunkSubscribe(this, 0, -1); // TODO: use player coords
    m_server->chunkSubscribe(this, -1, -1); // TODO: use player coords

    set(mcCommandType(0x06));
    set(mcInt(10));
    set(mcInt(70));
    set(mcInt(10));


    /*
    set(mcCommandType(0x09));
    set(mcByte(0));
    set(mcByte(3));
    set(mcByte(0));
    set(mcShort(128));
    set(mcLong(1));*/

    set(mcCommandType(0x0D));
    set(mcDouble(75));
    set(mcDouble(77));
    set(mcDouble(75));
    set(mcDouble(75));
    set(mcFloat(0));
    set(mcFloat(0));
    set(mcByte(0));


    set(mcCommandType(0x04));
    set(mcLong(6000));

    set(mcCommandType(0x22));
    set(m_eid);
    set(mcInt(0));
    set(mcInt(70*32));
    set(mcInt(0));
    set(mcByte(0));
    set(mcByte(0));
}

void Client::handleHandshake(void)
{
    std::string username;
    if (!get(username)) return read();

    set(mcCommandType(0x02));
    if (m_server->authRequired()) {
        // TODO: support authentication
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

void Client::handleKeepAlive(void)
{
    mcInt id;
    if (!get(id)) return read();
    set(mcCommandType(0x00));
    set(id);
}

void Client::handleOnGround(void)
{
    mcByte onGround;
    if (!get(onGround)) return read();

    // TODO: do something
}

void Client::handlePlayerPositionUpdate(void)
{
    mcDouble x;
    if (!get(x)) return read();
    mcDouble y;
    if (!get(y)) return read();
    mcDouble stance;
    if (!get(stance)) return read();
    mcDouble z;
    if (!get(z)) return read();
    mcByte onGround;
    if (!get(onGround)) return read();

    // TODO: do something here

    LOG_DEBUG << x << " " << y << " " << z << "\n";
}

void Client::handlePlayerLook(void)
{
    mcFloat yaw;
    if (!get(yaw)) return read();
    mcFloat pitch;
    if (!get(pitch)) return read();
    mcByte onGround;
    if (!get(onGround)) return read();
}

void Client::handlePlayerPosition(void)
{
    mcDouble x;
    if (!get(x)) return read();
    mcDouble y;
    if (!get(y)) return read();
    mcDouble stance;
    if (!get(stance)) return read();
    mcDouble z;
    if (!get(z)) return read();
    mcFloat yaw;
    if (!get(yaw)) return read();
    mcFloat pitch;
    if (!get(pitch)) return read();
    mcByte onGround;
    if (!get(onGround)) return read();

    // TODO: do something here
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

bool Client::get(mcUByte & b)
{
    return getHelper(b, m_data, m_dataItem, m_incoming, m_readNeeded);
}

bool Client::get(mcShort & s)
{
    bool ret = getHelper(s, m_data, m_dataItem, m_incoming, m_readNeeded);
    s = ntohs(s);
    return ret;
}

bool Client::get(mcUShort & s)
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

bool Client::get(mcUInt & i)
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

bool Client::get(mcULong & l)
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

void Client::set(mcUByte b)
{
    setHelper(b, m_outgoing);
}

void Client::set(mcShort s)
{
    setHelper(htons(s), m_outgoing);
}

void Client::set(mcUShort s)
{
    setHelper(htons(s), m_outgoing);
}

void Client::set(mcInt i)
{
    setHelper(htonl(i), m_outgoing);
}

void Client::set(mcUInt i)
{
    setHelper(htonl(i), m_outgoing);
}

void Client::set(mcLong l)
{
    setHelper(htonll(l), m_outgoing);
}

void Client::set(mcULong l)
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

void Client::set(const std::string & str)
{
    std::u16string utf16;
    
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16));
    set(mcShort(utf16.length()));

    for (mcShort c: utf16) {
        set(c);
    }
}
