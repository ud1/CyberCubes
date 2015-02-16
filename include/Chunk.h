#ifndef CHUNK_H
#define CHUNK_H

#include "Math.h"
#include <stdint.h>

typedef signed char LightValue;
typedef uint16_t CubeType;
constexpr int MAX_LIGHT = 63;
constexpr size_t CHUNK_SIZE_LOG2 = 5;
constexpr size_t CHUNK_SIZE = 1 << CHUNK_SIZE_LOG2;
constexpr size_t CHUNK_SIZE_M1 = CHUNK_SIZE - 1;

constexpr int eucModChunk(int v)
{
	return v & CHUNK_SIZE_M1;
}

constexpr int eucDivChunk(int v)
{
	return v >> CHUNK_SIZE_LOG2;
}


struct Chunk
{
	Chunk *u, *d, *l, *r, *b, *f;
	bool isDummy = false;
	bool isDirty = false;

    CubeType cubes[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
    LightValue light[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
    LightValue sunLight[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    Chunk();

    void put(const math::ivec3 &p, int type);
    bool hasEdge(const math::ivec3 &p, Dir dir) const;
    void updateLight();
    void updateSunLight();
    int updateSunLightIter();


    template<bool sun>
    LightValue lightAt(const math::ivec3 &p) const;

    template<bool sun>
    LightValue rawLightAt(const math::ivec3 &p) const
    {
		return (sun ? sunLight : light)[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
    }

    template<bool sun>
    LightValue& rawLightRefAt(const math::ivec3 &p)
    {
		return (sun ? sunLight : light)[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
    }

	template<bool sun>
    LightValue &lightRefAt(const math::ivec3 &p);

    CubeType cubeAt(const math::ivec3 &p) const;
    int getSunLight(int x, int y) const;
};

#endif // CHUNK_H
