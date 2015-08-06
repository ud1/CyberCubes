#ifndef PLAYER_H
#define PLAYER_H

#include "Math.h"
#include "Chunk.h"

#include <unordered_set>

class World;

namespace cyberCubes {

class Player
{
public:
	Player(World *world);
	~Player();
	
	void updatePosition(const math::vec3 &position);
	
	typedef std::unordered_set<IntCoord, IntCoordHasher> ChunkPositionSet;
	
	const ChunkPositionSet &getChunks() const
	{
		return chunks;
	}
	
private:
	math::vec3 position;
	ChunkPositionSet chunks;
	
	World *world;
};

}
#endif // PLAYER_H
