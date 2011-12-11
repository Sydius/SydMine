#include "chunkmanager.hpp"

#include "client.hpp"

ChunkManager::ChunkManager(const std::string & worldDir)
{
}

void ChunkManager::subscribe(Client * client, int x, int z)
{
    Chunk chunk;

    client->sendInitChunk(x, z, true);
    client->sendChunk(x, z, chunk);
}
