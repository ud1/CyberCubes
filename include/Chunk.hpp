#ifndef CHUNK_H
#define CHUNK_H

#include "Math.hpp"
#include <atomic>
#include <unordered_map>
#include "types.hpp"

constexpr int MAX_LIGHT = 31;
constexpr float MAX_LIGHT_F = MAX_LIGHT;
constexpr size_t CHUNK_SIZE_LOG2 = 5;
constexpr size_t CHUNK_SIZE = 1 << CHUNK_SIZE_LOG2;
constexpr int CHUNK_SIZE_I = CHUNK_SIZE;
constexpr float CHUNK_SIZE_F = CHUNK_SIZE;
constexpr size_t CHUNK_SIZE_M1 = CHUNK_SIZE - 1;
constexpr int CHUNK_SIZE_M1_I = CHUNK_SIZE_M1;

enum LightType
{
	LIGHT_SUN = 0,
	LIGHT_R = 1,
	LIGHT_G = 2,
	LIGHT_B = 3,
	LIGHT_SUNRGBA = 4
};
constexpr size_t LIGHT_COUNT = 4;

using Tick = size_t;

template<LightType t>
struct LightVal_t
{
	using type = LightValue;
	using ftype = float;
};

template<>
struct LightVal_t<LIGHT_SUNRGBA>
{
	using type = math::ivec4;
	using ftype = math::vec4;
};

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

inline math::ivec3 floorCoord(const math::vec3 &c)
{
	math::vec3 f = math::floor(c);
	return math::ivec3(f.x, f.y, f.z);
}

inline math::vec3 i2f(const math::ivec3 &c)
{
	return math::vec3(c.x, c.y, c.z);
}

inline math::ivec3 eucDivChunk(const math::ivec3 &c)
{
	return  math::ivec3(eucDivChunk(c.x), eucDivChunk(c.y), eucDivChunk(c.z));
}

inline math::ivec3 eucModChunk(const math::ivec3 &c)
{
	return  math::ivec3(eucModChunk(c.x), eucModChunk(c.y), eucModChunk(c.z));
}

struct Chunk;

template<LightType lt>
LightVal<lt> rawLightAt(const Chunk &ch, const math::ivec3 &p);

struct SunLightPropagationLayer
{
	SunLightPropagationLayer();
	
	unsigned char numBlocks[CHUNK_SIZE*CHUNK_SIZE];
	
	std::atomic_bool isLoaded, isCannotBeLoaded;
	bool needToPersist = false;
	
	unsigned char &valueAt(int x, int y)
	{
		return numBlocks[(x * CHUNK_SIZE) + y];
	}
	
	bool hasZeros() const;
};

struct SunLightPropagationSum
{
	SunLightPropagationSum();
	unsigned numBlocks[CHUNK_SIZE*CHUNK_SIZE];
	
	unsigned valueAt(int x, int y)
	{
		return numBlocks[(x * CHUNK_SIZE) + y];
	}
	
	void add(const SunLightPropagationLayer &l);
};

struct Chunk
{
	Chunk *u, *d, *l, *r, *b, *f;
	bool isDummy = false;
	bool isDirty = false;
	bool needToPersist = false;
	unsigned opaqueBlockCount, nonOpaqueBlockCount;
	
	std::atomic_bool isLoaded, isLighted, isSunLighted;
	bool sunLightRecalculating;
	Tick touchTick;

	CubeType cubes[CHUNK_SIZE *CHUNK_SIZE *CHUNK_SIZE];
	LightValue light[LIGHT_COUNT][CHUNK_SIZE *CHUNK_SIZE *CHUNK_SIZE];
	std::unordered_map<uint16_t, BlockData> blockData;
	
	SunLightPropagationLayer *slpl;

	Chunk();

	void put(const math::ivec3 &p, CubeType type);
	bool hasEdge(const math::ivec3 &p, Dir dir, bool ignore_neib_chunks) const;
	void updateLight();

	template<LightType lt>
	LightVal<lt> lightAt(const math::ivec3 &p) const;

	template<LightType lt>
	LightVal<lt> rawLightAt(const math::ivec3 &p) const
	{
		return ::rawLightAt<lt>(*this, p);
	}

	template<LightType lt>
	LightValue &rawLightRefAt(const math::ivec3 &p)
	{
		return light[lt][((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
	}

	template<LightType lt>
	LightValue &lightRefAt(const math::ivec3 &p);

	CubeType cubeAt(const math::ivec3 &p) const;
	CubeType rawCubeAt(const math::ivec3 &p) const;
	BlockData getBlockData(const math::ivec3 &p) const;
	void setBlockData(const math::ivec3 &p, BlockData data);
	static unsigned char decodeRotation(BlockData data);
	
	void computeSunLightPropagationLayer(SunLightPropagationLayer &layer) const;
	
	void recalcBlockCount();
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


using IntCoord = math::ivec3;

struct IntCoordHasher
{
	std::size_t operator()(const IntCoord &k) const
	{
		return (k.x * 256 * 256) + (k.y * 256) + k.z;
	}
};


#endif // CHUNK_H
