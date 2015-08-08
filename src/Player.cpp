#include "Player.h"
#include "World.h"

namespace cyberCubes {
	
Player::Player(World *world_)
{
	world = world_;
}

Player::~Player()
{

}

void Player::updatePosition(const math::vec3 &position_)
{
	position = position_;
	
	chunks.clear();
	
	IntCoord p = eucDivChunk(floorCoord(position));
	
	constexpr int RAD = 8;
	for (int x = -RAD; x <= RAD; ++x)
	{
		for (int y = -RAD; y <= RAD; ++y)
		{
			for (int z = -RAD; z <= RAD; ++z)
			{
				IntCoord pos = p + IntCoord(x, y, z);
				
				if (world->isChunkCoordValid(pos))
					chunks.insert(pos);
			}
		}
	}
}

}