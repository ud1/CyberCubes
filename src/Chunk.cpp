#include "Chunk.hpp"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <boost/timer/timer.hpp>
#include <cassert>
#include "blockType.hpp"

SunLightPropagationLayer::SunLightPropagationLayer() : isLoaded(false), isCannotBeLoaded(false)
{
	
}

bool SunLightPropagationLayer::hasZeros() const
{
	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; ++i)
	{
		if (!numBlocks[i])
			return true;
	}

	return false;
}

SunLightPropagationSum::SunLightPropagationSum()
{
	memset(numBlocks, 0, sizeof(numBlocks));
}

void SunLightPropagationSum::add(const SunLightPropagationLayer &l)
{
	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; ++i)
	{
		numBlocks[i] += l.numBlocks[i];
	}
}

Chunk::Chunk() : isLoaded(false), isLighted(false), isSunLighted(false)
{
	memset(cubes, 0, sizeof(cubes));
	memset(light, 0, sizeof(light));
	u = d = l = r = f = b = nullptr;
	touchTick = 0;
	opaqueBlockCount = 0;
	nonOpaqueBlockCount = 0;
	slpl = nullptr;
	sunLightRecalculating = false;
}

void Chunk::put(const math::ivec3 &p, CubeType type)
{
	CubeType &t = cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
	
	if (slpl)
	{
		if (isSunFullyTransparent(t) && !isSunFullyTransparent(type))
		{
			++slpl->valueAt(p.x, p.y);
		}
		else if (!isSunFullyTransparent(t) && isSunFullyTransparent(type))
		{

			--slpl->valueAt(p.x, p.y);
		}
	}
	
	if (isOpaque(t))
		--opaqueBlockCount;
	else
		++opaqueBlockCount;
	
	if (isOpaque(type))
		++opaqueBlockCount;
	else if (type > 0)
		++nonOpaqueBlockCount;
	
	t = type;
	needToPersist = true;
	if (slpl)
		slpl->needToPersist = true;
}


bool Chunk::hasEdge(const math::ivec3 &p, Dir dir) const
{
	CubeType t = rawCubeAt(p);
	if (!isSolid(t))
		return false;
	
	math::ivec3 p2 = p;
	if (dir == Dir::XN)
		--p2.x;
	else if (dir == Dir::XP)
		++p2.x;
	else if (dir == Dir::YN)
		--p2.y;
	else if (dir == Dir::YP)
		++p2.y;
	else if (dir == Dir::ZN)
		--p2.z;
	else if (dir == Dir::ZP)
		++p2.z;
	
	CubeType t2 = cubeAt(p2);
	return !isOpaque(t2);
}

void Chunk::computeSunLightPropagationLayer(SunLightPropagationLayer &layer) const
{
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_SIZE; ++y)
		{
			int num = 0;

			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				if (rawCubeAt(math::ivec3(x, y, z)) > 0)
					++num;
			}

			layer.valueAt(x, y) = num;
		}
	}
	layer.isLoaded.store(true);
}

void Chunk::updateLight()
{
	memset(light, 0, sizeof(light));
}

template <LightType lt>
inline LightVal<lt> getDummyLight()
{
	return lt == LIGHT_SUN ? MAX_LIGHT : 0;
}

template <>
inline LightVal<LIGHT_SUNRGBA> getDummyLight<LIGHT_SUNRGBA>()
{
	return math::ivec4 {MAX_LIGHT, 0, 0, 0};
}

template<LightType lt>
LightVal<lt> Chunk::lightAt(const math::ivec3 &p) const
{
	if (isDummy)
		return getDummyLight<lt>();

	if (p.x < 0)
		return l->lightAt<lt>(math::ivec3(p.x + CHUNK_SIZE, p.y, p.z));

	if (p.x >= (int) CHUNK_SIZE)
		return r->lightAt<lt>(math::ivec3(p.x - CHUNK_SIZE, p.y, p.z));

	if (p.y < 0)
		return b->lightAt<lt>(math::ivec3(p.x, p.y + CHUNK_SIZE, p.z));

	if (p.y >= (int) CHUNK_SIZE)
		return f->lightAt<lt>(math::ivec3(p.x, p.y - CHUNK_SIZE, p.z));

	if (p.z < 0)
		return d->lightAt<lt>(math::ivec3(p.x, p.y, p.z + CHUNK_SIZE));

	if (p.z >= (int) CHUNK_SIZE)
		return u->lightAt<lt>(math::ivec3(p.x, p.y, p.z - CHUNK_SIZE));

	return rawLightAt<lt>(p);
}

CubeType Chunk::cubeAt(const math::ivec3 &p) const
{
	if (isDummy)
		return 1;

	if (p.x < 0)
		return l->cubeAt(math::ivec3(p.x + CHUNK_SIZE, p.y, p.z));

	if (p.x >= (int) CHUNK_SIZE)
		return r->cubeAt(math::ivec3(p.x - CHUNK_SIZE, p.y, p.z));

	if (p.y < 0)
		return b->cubeAt(math::ivec3(p.x, p.y + CHUNK_SIZE, p.z));

	if (p.y >= (int) CHUNK_SIZE)
		return f->cubeAt(math::ivec3(p.x, p.y - CHUNK_SIZE, p.z));

	if (p.z < 0)
		return d->cubeAt(math::ivec3(p.x, p.y, p.z + CHUNK_SIZE));

	if (p.z >= (int) CHUNK_SIZE)
		return u->cubeAt(math::ivec3(p.x, p.y, p.z - CHUNK_SIZE));

	return rawCubeAt(p);
}

template LightValue Chunk::lightAt<LIGHT_SUN>(const math::ivec3 &p) const;
template LightValue Chunk::lightAt<LIGHT_R>(const math::ivec3 &p) const;
template LightValue Chunk::lightAt<LIGHT_G>(const math::ivec3 &p) const;
template LightValue Chunk::lightAt<LIGHT_B>(const math::ivec3 &p) const;
template math::ivec4 Chunk::lightAt<LIGHT_SUNRGBA>(const math::ivec3 &p) const;

CubeType Chunk::rawCubeAt(const math::ivec3 &p) const
{
	return cubes[((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z];
}

void Chunk::recalcBlockCount()
{
	unsigned opaqueCount = 0;
	unsigned nonOpaqueCount = 0;

	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++i)
	{
		CubeType t = cubes[i];
		if (t > 0)
		{
			if (isOpaque(t))
				++opaqueCount;
			else
				++nonOpaqueCount;
		}
	}

	opaqueBlockCount = opaqueCount;
	nonOpaqueBlockCount = nonOpaqueCount;
}
