#ifndef PLAYER_H
#define PLAYER_H

#include "Math.hpp"
#include "Chunk.hpp"

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
	
	const math::vec3 &getPosition() const
	{
		return position;
	}
	
	float getFogFar() const
	{
		return (viewDistance - 0.8f) * CHUNK_SIZE_F;
	}
private:
	math::vec3 position;
	ChunkPositionSet chunks;
	
	World *world;
	int viewDistance;
};

}
#endif // PLAYER_H
