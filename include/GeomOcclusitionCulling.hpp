#ifndef GEOM_OCCLUSISION_CULLING_HPP
#define GEOM_OCCLUSISION_CULLING_HPP

#include <memory>
#include <vector>

#include "Math.hpp"
#include "Chunk.hpp"

namespace cyberCubes {

class GeomOcclusitionCulling
{
public:
	GeomOcclusitionCulling(const math::vec3 &cameraPos);
	
	void addOccluder(const math::vec3 &chunkPos);
	bool isOccluded(const math::vec3 &chunkPos) const;
	
private:
	std::vector<math::vec3> chunkPostitions;
	math::vec3 cameraPos;
};

}

#endif
