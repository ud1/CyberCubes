#include "Chunk.h"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <boost/timer/timer.hpp>
#include <cassert>

Chunk::Chunk()
{
    memset(cubes, 0, sizeof(cubes));
    u = d = l = r = f = b = NULL;
}

void Chunk::put(const math::ivec3 &p, int type)
{
    cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z] = type;
}


bool Chunk::hasEdge(const math::ivec3 &p, Dir dir) const
{
    if (!cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z])
        return false;

    if ((dir == Dir::XN && p.x == 0) ||
            (dir == Dir::XP && p.x == (CHUNK_SIZE - 1)) ||
            (dir == Dir::YN && p.y == 0) ||
            (dir == Dir::YP && p.y == (CHUNK_SIZE - 1)) ||
            (dir == Dir::ZN && p.z == 0) ||
            (dir == Dir::ZP && p.z == (CHUNK_SIZE - 1)))
    {
        return true;
    }

    switch (dir)
    {
    case Dir::XN:
        return cubes[(((p.x - 1) * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z] == 0;
    case Dir::XP:
        return cubes[(((p.x + 1) * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z] == 0;
    case Dir::YN:
        return cubes[((p.x * CHUNK_SIZE) + (p.y - 1)) * CHUNK_SIZE + p.z] == 0;
    case Dir::YP:
        return cubes[((p.x * CHUNK_SIZE) + (p.y + 1)) * CHUNK_SIZE + p.z] == 0;
    case Dir::ZN:
        return cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z - 1] == 0;
    case Dir::ZP:
        return cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z + 1] == 0;
    }
    assert(false);
    return false;
}

int Chunk::getSunLight(int x, int y) const
{
	if (isDummy)
		return MAX_LIGHT;

	for (int z = CHUNK_SIZE; z --> 0;)
	{
		if (cubeAt(math::ivec3(x, y, z)))
			return 0;
	}

	return u->getSunLight(x, y);
}

void Chunk::updateLight()
{
	memset(light, 0, sizeof(light));
}

void Chunk::updateSunLight()
{
    memset(sunLight, 0, sizeof(sunLight));

    for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
        {
			if (!u->getSunLight(x, y))
				continue;

            for (unsigned int z = CHUNK_SIZE; z --> 0;)
            {
                if (z > 0 && cubes[((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z - 1])
                {
                    sunLight[((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z] = MAX_LIGHT;
                    break;
                }
                else
                {
                    sunLight[((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z] = MAX_LIGHT;
                }
            }
        }
    }
}

int Chunk::updateSunLightIter()
{
	int res = 0;
    for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
        {
            for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
            {
                if (!cubes[((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z])
                {
                    LightValue v1 = lightAt<true>(math::ivec3(x-1, y, z));
                    LightValue v2 = lightAt<true>(math::ivec3(x+1, y, z));
                    LightValue v3 = lightAt<true>(math::ivec3(x, y-1, z));
                    LightValue v4 = lightAt<true>(math::ivec3(x, y+1, z));
                    LightValue v5 = lightAt<true>(math::ivec3(x, y, z-1));
                    LightValue v6 = lightAt<true>(math::ivec3(x, y, z+1));

                    LightValue v = std::max(std::max(std::max(std::max(std::max(v1, v2), v3), v4), v5), v6) - 1;
                    if (lightAt<true>(math::ivec3(x, y, z)) < v)
                    {
                        sunLight[((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z] = v;
                        ++res;
					}
                }
            }
        }
    }
    return res;
}

template<bool sun>
LightValue Chunk::lightAt(const math::ivec3 &p) const
{
	if (isDummy)
		return sun ? MAX_LIGHT : 0;
	/*if (p.z < 0 && d->isDummy || p.z >= CHUNK_SIZE && u->isDummy)
        return MAX_LIGHT;

	if (p.x < 0 && l->isDummy || p.x >= CHUNK_SIZE && r->isDummy ||
		p.y < 0 && b->isDummy || p.y >= CHUNK_SIZE && f->isDummy)
        return MAX_LIGHT;*/

	if (p.x < 0)
		return l->lightAt<sun>(math::ivec3(p.x + CHUNK_SIZE, p.y, p.z));

	if (p.x >= (int) CHUNK_SIZE)
		return r->lightAt<sun>(math::ivec3(p.x - CHUNK_SIZE, p.y, p.z));

	if (p.y < 0)
		return b->lightAt<sun>(math::ivec3(p.x, p.y + CHUNK_SIZE, p.z));

	if (p.y >= (int) CHUNK_SIZE)
		return f->lightAt<sun>(math::ivec3(p.x, p.y - CHUNK_SIZE, p.z));

	if (p.z < 0)
		return d->lightAt<sun>(math::ivec3(p.x, p.y, p.z + CHUNK_SIZE));

	if (p.z >= (int) CHUNK_SIZE)
		return u->lightAt<sun>(math::ivec3(p.x, p.y, p.z - CHUNK_SIZE));

    if (cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z])
        return 0;
    return rawLightAt<sun>(p);
}

template LightValue Chunk::lightAt<true>(const math::ivec3 &p) const;
template LightValue Chunk::lightAt<false>(const math::ivec3 &p) const;

CubeType Chunk::cubeAt(const math::ivec3 &p) const
{
    return cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
}
