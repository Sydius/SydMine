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

#pragma once

#include <cstdint>

#define CHUNK_X_SIZE 16
#define CHUNK_Y_SIZE 128
#define CHUNK_Z_SIZE 16

class Chunk
{
    public:
        typedef int Coord;

        Chunk();

        int getDataSize(void) const
        {
            return sizeof(m_data.data);
        }

        const uint8_t * getData(void) const
        {
            return m_data.data;
        }

        int getXSize(void) const
        {
            return CHUNK_X_SIZE;
        }

        int getYSize(void) const
        {
            return CHUNK_Y_SIZE;
        }

        int getZSize(void) const
        {
            return CHUNK_Z_SIZE;
        }

        void setBlockType(int x, int y, int z, uint8_t type)
        {
            m_data.blocks.type[x * getZSize() * getYSize() + z * getYSize() + y] = type;
        }

    private:
    union {
        struct {
            uint8_t type[CHUNK_X_SIZE*CHUNK_Z_SIZE*CHUNK_Y_SIZE];
            uint8_t metadata[CHUNK_X_SIZE*CHUNK_Z_SIZE*CHUNK_Y_SIZE/2];
            uint8_t blockLight[CHUNK_X_SIZE*CHUNK_Z_SIZE*CHUNK_Y_SIZE/2];
            uint8_t skyLight[CHUNK_X_SIZE*CHUNK_Z_SIZE*CHUNK_Y_SIZE/2];
        } blocks;
        uint8_t data[int(CHUNK_X_SIZE*CHUNK_Z_SIZE*CHUNK_Y_SIZE*2.5)];
    } m_data;
};
