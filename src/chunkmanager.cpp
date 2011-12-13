#include "chunkmanager.hpp"

#include "client.hpp"

ChunkManager::ChunkManager(const std::string & worldDir)
{
}

void ChunkManager::subscribe(Client * client, int x, int z)
{
    Chunk chunk;

    for (int cx = x - 3; cx <= x + 3; cx++) {
        for (int cz = z - 3; cz <= z + 3; cz++) {
            client->sendInitChunk(cx, cz, true);
            client->sendChunk(cx, cz, chunk);
        }
    }
}
