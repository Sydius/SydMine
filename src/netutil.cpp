/*  Coypright (C) 2011 Christopher Allen Ogden
 *
 *     This file is part of SydMine.
 *
 *  SydMine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SydMine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SydMine.  If not, see <http://www.gnu.org/licenses/>.
 */

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
