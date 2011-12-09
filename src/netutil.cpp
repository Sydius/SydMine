#include "netutil.hpp"

#include <boost/asio.hpp>

static bool diffEndian(void)
{
    static const int num = 26;
    if (*reinterpret_cast<const char*>(&num) == num) {
        return true;
    }
    return false;
}

uint64_t htonll(uint64_t value)
{
    if (diffEndian()) {
        uint32_t highPart = htonl(static_cast<uint32_t>(value >> 32));
        uint32_t lowPart = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

        return (static_cast<uint64_t>(lowPart) << 32) | highPart;
    }
    return value;
}

uint64_t ntohll(uint64_t value)
{
    if (diffEndian()) {
        uint32_t highPart = ntohl(static_cast<uint32_t>(value >> 32));
        uint32_t lowPart = ntohl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

        return (static_cast<uint64_t>(lowPart) << 32) | highPart;
    }
    return value;
}
