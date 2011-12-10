#pragma once

#include <cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128
#define CHUNK_DEPTH 16

class Chunk
{
    public:
        Chunk();
        
    private:
    union {
        struct {
            uint8_t type[CHUNK_WIDTH*CHUNK_DEPTH*CHUNK_HEIGHT];
            uint8_t metadata[CHUNK_WIDTH*CHUNK_DEPTH*CHUNK_HEIGHT/2];
            uint8_t blockLight[CHUNK_WIDTH*CHUNK_DEPTH*CHUNK_HEIGHT/2];
            uint8_t skyLight[CHUNK_WIDTH*CHUNK_DEPTH*CHUNK_HEIGHT/2];
        } blocks;
        uint8_t data[int(CHUNK_WIDTH*CHUNK_DEPTH*CHUNK_HEIGHT*2.5)];
    } m_data;
};
