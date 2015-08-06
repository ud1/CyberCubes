#ifndef WORLDPROVIDER_H
#define WORLDPROVIDER_H

#include "Chunk.h"
#include "Math.h"

class WorldProvider
{
public:

	bool save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite);
	bool load(Chunk &chunk, const math::ivec3 &pos);
	//bool load(SunLightPropagationLayer &layer, const math::ivec3 &pos);

	void fill(Chunk &chunk, const math::ivec3 &pos);
};

#endif // WORLDPROVIDER_H
