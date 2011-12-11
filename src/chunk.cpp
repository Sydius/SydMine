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
            setBlockType(x, 70, z, 2);
        }
    }
}
