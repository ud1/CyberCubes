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
	LIGHT_SUNRGBA = 4
};
constexpr size_t LIGHT_COUNT = 4;

template<LightType t>
struct LightVal_t {using type = LightValue; using ftype = float;};

template<>
struct LightVal_t<LIGHT_SUNRGBA> {using type = math::ivec4; using ftype = math::vec4;};

template<LightType t>
using LightVal = typename LightVal_t<t>::type;

template<LightType t>
using LightValF = typename LightVal_t<t>::ftype;

template <typename T>
inline T defVal()
{
	T t = {};
	return t;
}

template <>
inline math::vec4 defVal<math::vec4>()
{
	return math::vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

template <typename T>
inline bool isNotNull(const T &t)
{
	return t > 0;
}

template <>
inline bool isNotNull<math::ivec4>(const math::ivec4 &t)
{
	return t.x > 0 || t.y > 0 || t.z > 0 || t.w > 0;
}

constexpr int eucModChunk(int v)
{
	return v & CHUNK_SIZE_M1;
}

constexpr int eucDivChunk(int v)
{
	return v >> CHUNK_SIZE_LOG2;
}

struct Chunk;

template<LightType lt>
LightVal<lt> rawLightAt(const Chunk &ch, const math::ivec3 &p);

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
    LightVal<lt> lightAt(const math::ivec3 &p) const;

    template<LightType lt>
    LightVal<lt> rawLightAt(const math::ivec3 &p) const
    {
		return ::rawLightAt<lt>(*this, p);
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

template<LightType lt>
inline LightVal<lt> rawLightAt(const Chunk &ch, const math::ivec3 &p)
{
	return ch.light[lt][((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
}

template<>
inline math::ivec4 rawLightAt<LIGHT_SUNRGBA>(const Chunk &ch, const math::ivec3 &p)
{
	size_t pos = ((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z;
	return math::ivec4(ch.light[LIGHT_SUN][pos], ch.light[LIGHT_R][pos], ch.light[LIGHT_G][pos], ch.light[LIGHT_B][pos]);
}


#endif // CHUNK_H
