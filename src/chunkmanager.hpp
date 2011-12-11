#pragma once

#include <string>
#include "chunk.hpp"

class Client;

class ChunkManager
{
    public:
        ChunkManager(const std::string & worldDir);

        void subscribe(Client * client, int x, int z);
};
