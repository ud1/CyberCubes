#include "GeomOcclusitionCulling.hpp"
#include "Chunk.hpp"
#include <vector>

bool chunkIntersection(const math::vec3 &boxCorner, const math::vec3 &r0, const math::vec3 &invDir)
{
    math::vec3 relPos = boxCorner - r0;
    math::vec3 t1 = relPos * invDir;
    math::vec3 t2 = (relPos + math::vec3(CHUNK_SIZE_F, CHUNK_SIZE_F, CHUNK_SIZE_F)) * invDir;
    math::vec3 mint1t2 = math::min(t1, t2);
    math::vec3 maxt1t2 = math::max(t1, t2);
    float tmin = math::max(mint1t2.x, math::max(mint1t2.y, mint1t2.z));
    float tmax = math::min(maxt1t2.x, math::min(maxt1t2.y, maxt1t2.z));
    return tmax >= tmin && tmax > 0.0f && tmax < 1.0f;
}

namespace cyberCubes {
	
const math::vec3 chunkCorners[8] = {
	{0.0f, 0.0f, 0.0f},
	{CHUNK_SIZE_F, 0.0f, 0.0f},
	{0.0f, CHUNK_SIZE_F, 0.0f},
	{CHUNK_SIZE_F, CHUNK_SIZE_F, 0.0f},
	{0.0f, 0.0f, CHUNK_SIZE_F},
	{CHUNK_SIZE_F, 0.0f, CHUNK_SIZE_F},
	{0.0f, CHUNK_SIZE_F, CHUNK_SIZE_F},
	{CHUNK_SIZE_F, CHUNK_SIZE_F, CHUNK_SIZE_F}
};


void GeomOcclusitionCulling::addOccluder(const math::vec3 &chunkPos)
{
	chunkPostitions.push_back(chunkPos);
}

bool GeomOcclusitionCulling::isOccluded(const math::vec3 &chunkPos) const
{
	for (const math::vec3 &p : chunkPostitions)
	{
		bool res = true;
		for (int i = 0; i < 8; ++i)
		{
			const math::vec3 &c = chunkCorners[i];
			math::vec3 cp = chunkPos + c;
			math::vec3 inv = 1.0f/(cameraPos - cp);
				
			if (!chunkIntersection(p, cp, inv))
			{
				res = false;
				break;
			}
		}
		
		if (res)
			return true;
	}
	
	return false;
}

GeomOcclusitionCulling::GeomOcclusitionCulling(const math::vec3 &cameraPos)
{
	this->cameraPos = cameraPos;
}

}