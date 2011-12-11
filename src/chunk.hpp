#pragma once

#include <cstdint>

#define CHUNK_X_SIZE 16
#define CHUNK_Y_SIZE 128
#define CHUNK_Z_SIZE 16

class Chunk
{
    public:
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
