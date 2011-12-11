#include "chunk.hpp"

#include <string.h>

Chunk::Chunk()
    : m_data()
{
    memset(m_data.data, 0, sizeof(m_data.data));
}
