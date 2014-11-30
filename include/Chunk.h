#ifndef CHUNK_H
#define CHUNK_H

#include "Math.h"
#include <stdint.h>

typedef signed char LightValue;
typedef uint16_t CubeType;
const int MAX_LIGHT = 127;
const size_t CHUNK_SIZE = 32;

struct Chunk
{
	Chunk *u, *d, *l, *r, *b, *f;
	bool isDummy = false;
	bool isDirty = false;

    CubeType cubes[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
    LightValue light[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    Chunk();

    void put(const math::ivec3 &p, int type);
    bool hasEdge(const math::ivec3 &p, Dir dir) const;
    void updateSunLight();
    int updateLightIter();
    LightValue lightAt(const math::ivec3 &p) const;
    LightValue &lightRefAt(const math::ivec3 &p);
    CubeType cubeAt(const math::ivec3 &p) const;
    int getSunLight(int x, int y) const;
};

#endif // CHUNK_H
