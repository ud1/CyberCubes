#include "Player.hpp"
#include "World.hpp"

namespace cyberCubes {
	
Player::Player(World *world_)
{
	world = world_;
	viewDistance = 5;
}

Player::~Player()
{

}

void Player::updatePosition(const math::vec3 &position_)
{
	position = position_;
	
	chunks.clear();
	
	IntCoord p = eucDivChunk(floorCoord(position));
	
	for (int x = -viewDistance; x <= viewDistance; ++x)
	{
		for (int y = -viewDistance; y <= viewDistance; ++y)
		{
			for (int z = -viewDistance; z <= viewDistance; ++z)
			{
				if (x * x + y * y + z * z <= (viewDistance + 1) * (viewDistance + 1))
				{
					IntCoord pos = p + IntCoord(x, y, z);
					
					if (world->isChunkCoordValid(pos))
						chunks.insert(pos);
				}
			}
		}
	}
}

}