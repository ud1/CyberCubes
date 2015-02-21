#ifndef CHUNK_H
#define CHUNK_H

#include "Math.h"
#include <stdint.h>

typedef signed char LightValue;
typedef uint16_t CubeType;
constexpr int MAX_LIGHT = 63;
constexpr float MAX_LIGHT_F = MAX_LIGHT;
constexpr size_t CHUNK_SIZE_LOG2 = 5;
constexpr size_t CHUNK_SIZE = 1 << CHUNK_SIZE_LOG2;
constexpr size_t CHUNK_SIZE_M1 = CHUNK_SIZE - 1;

enum LightType {
	LIGHT_SUN = 0,
	LIGHT_R = 1,
	LIGHT_G = 2,
	LIGHT_B = 3,
	LIGHT_COUNT = 4
};

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
    LightValue light[LIGHT_COUNT][CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    Chunk();

    void put(const math::ivec3 &p, int type);
    bool hasEdge(const math::ivec3 &p, Dir dir) const;
    void updateLight();
    void updateSunLight();
    int updateSunLightIter();


    template<LightType lt>
    LightValue lightAt(const math::ivec3 &p) const;

    template<LightType lt>
    LightValue rawLightAt(const math::ivec3 &p) const
    {
		return light[lt][((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
    }

    template<LightType lt>
    LightValue& rawLightRefAt(const math::ivec3 &p)
    {
		return light[lt][((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
    }

	template<LightType lt>
    LightValue &lightRefAt(const math::ivec3 &p);

    CubeType cubeAt(const math::ivec3 &p) const;
    int getSunLight(int x, int y) const;
};

#endif // CHUNK_H
