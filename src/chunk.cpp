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

#include "chunk.hpp"

#include <string.h>

Chunk::Chunk()
    : m_data()
{
    memset(m_data.data, 0, sizeof(m_data.data));
    memset(m_data.blocks.blockLight, 0xFF, sizeof(m_data.blocks.blockLight));
    memset(m_data.blocks.skyLight, 0xFF, sizeof(m_data.blocks.skyLight));

    for (int z = 0; z < 16; z++) {
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 71; y++) {
                setBlockType(x, y, z, (y < 70) ? 3 : 2);
            }
        }
    }
}
