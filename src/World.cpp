#include "World.h"
#include "Chunk.h"
#include "RenderData.h"
#include "Frustum.h"
#include <iostream>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>
#include <cmath>
#include <libnoise/noise.h>
#include <boost/timer/timer.hpp>
#include "Block.h"

#include <fstream>

#include <chrono>
#include <thread>

const int S = 4;
const int SZ = 9;

//std::ofstream lout("lout.txt", std::ofstream::out | std::ofstream::trunc);

void checkGLError(const char *str);

World::World()
{
	_renderTick = 0;
	_maxRenderDataMemoryUse = 100 * 1024 * 1024;
	_maxChunksMemoryUse = 400 * 1024 * 1024;
	
	constexpr int W = 100000;
	minWorldChunkCoord = IntCoord(-W, -W, 0);
	maxWorldChunkCoord = IntCoord(W, W, SZ);
	
	_isLoadingThreadRunned.test_and_set();
	_loadChunksThread.reset(new std::thread(&World::_loadChunks, this));
	
	_isLightingThreadRunned.test_and_set();
	_lightingChunksThread.reset(new std::thread(&World::_lightChunks, this));
}

World::~World()
{
	_isLoadingThreadRunned.clear();
	_pendingChunkCoords.push(ChunkLoadRequest());
	
	_isLightingThreadRunned.clear();
	_lightingChunkCoords.push(ChunkLightUpRequest());
	
	_loadChunksThread->join();
	_lightingChunksThread->join();
	
	for (ChunkMap::const_iterator it = chunks.begin(); it != chunks.end(); ++it)
	{
		worldProvider.save(*it->second.get(), it->first, true);
	}
}

bool World::isChunkCoordValid(const IntCoord &p) const
{
	return
		minWorldChunkCoord.z <= p.z && p.z < maxWorldChunkCoord.z && 
		minWorldChunkCoord.x <= p.x && p.x < maxWorldChunkCoord.x &&
		minWorldChunkCoord.y <= p.y && p.y < maxWorldChunkCoord.y;
}

void World::_loadChunks()
{
	while(true)
	{
		ChunkLoadRequest request = _pendingChunkCoords.pop();
		
		if (!_isLoadingThreadRunned.test_and_set())
			break;
		
		worldProvider.fill(*request.chunk, request.chunkCoord);
	}
	_isLoadingThreadRunned.clear();
}

void updateBlockLightVector(Chunk *chunk, const math::ivec3 &p, const math::ivec3 &chunkPos,
			    std::vector<AddedBlockLight> &addedBlocks)
{
	CubeType cube = chunk->cubeAt(p);

	if (cube)
	{
		const Block *block = Block::get(cube);

		if (block->lightValueR)
			chunk->rawLightRefAt<LIGHT_R>(p) = block->lightValueR;
		if (block->lightValueG)
			chunk->rawLightRefAt<LIGHT_G>(p) = block->lightValueG;
		if (block->lightValueB)
			chunk->rawLightRefAt<LIGHT_B>(p) = block->lightValueB;
	}
	
	if (chunk->rawLightRefAt<LIGHT_R>(p) > 0 || chunk->rawLightRefAt<LIGHT_G>(p) > 0 || chunk->rawLightRefAt<LIGHT_B>(p) > 0)
	{
		AddedBlockLight lb;
		lb.position = p + chunkPos;
		lb.lightColor[LIGHT_R] = chunk->rawLightRefAt<LIGHT_R>(p);
		lb.lightColor[LIGHT_G] = chunk->rawLightRefAt<LIGHT_G>(p);
		lb.lightColor[LIGHT_B] = chunk->rawLightRefAt<LIGHT_B>(p);
		addedBlocks.push_back(lb);
		
		//lout << "UBLV " << lb.position.x << " " << lb.position.y << " " << lb.position.z << " " << (int) lb.lightColor[LIGHT_R] << " " << (int) lb.lightColor[LIGHT_G] << " " << (int) lb.lightColor[LIGHT_B] << std::endl;
	}
}

void World::_lightChunks()
{
	while(true)
	{
		ChunkLightUpRequest request = _lightingChunkCoords.pop();
		
		if (!_isLightingThreadRunned.test_and_set())
			break;
		
		std::vector<AddedBlockLight> addedBlocks;
		std::vector<math::ivec3> removedBlocks;
		
		//lout << "REQS1 " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		
		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
					updateBlockLightVector(request.chunk, math::ivec3(x, y, z), request.chunkCoord * CHUNK_SIZE_I, addedBlocks);
				}	
			}	
		}
		
		//lout << "REQS2 " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		
		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				math::ivec3 chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.z -= CHUNK_SIZE_I;
				if (!request.chunk->d->isDummy)
					updateBlockLightVector(request.chunk->d, math::ivec3(x, y, CHUNK_SIZE_I - 1), chunkPos, addedBlocks);
				
				chunkPos.z += 2 * CHUNK_SIZE_I;
				if (!request.chunk->u->isDummy)
					updateBlockLightVector(request.chunk->u, math::ivec3(x, y, 0), chunkPos, addedBlocks);
				
				chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.x -= CHUNK_SIZE_I;
				if (!request.chunk->l->isDummy)
					updateBlockLightVector(request.chunk->l, math::ivec3(CHUNK_SIZE_I - 1, x, y), chunkPos, addedBlocks);
				
				chunkPos.x += 2 * CHUNK_SIZE_I;
				if (!request.chunk->r->isDummy)
					updateBlockLightVector(request.chunk->r, math::ivec3(0, x, y), chunkPos, addedBlocks);
				
				
				chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.y -= CHUNK_SIZE_I;
				if (!request.chunk->b->isDummy)
					updateBlockLightVector(request.chunk->b, math::ivec3(x, CHUNK_SIZE_I - 1, y), chunkPos, addedBlocks);
				
				chunkPos.y += 2 * CHUNK_SIZE_I;
				if (!request.chunk->f->isDummy)
					updateBlockLightVector(request.chunk->f, math::ivec3(x, 0, y), chunkPos, addedBlocks);
			}
		}
		
		//lout << "UPD " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		
		if (!addedBlocks.empty())
		{
			updateLight<LIGHT_R>(addedBlocks, removedBlocks);
			updateLight<LIGHT_G>(addedBlocks, removedBlocks);
			updateLight<LIGHT_B>(addedBlocks, removedBlocks);
		}
		
		//lout << "REQF " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		
		request.chunk->isLighted.store(true);
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		
	}
	_isLightingThreadRunned.clear();
}

void World::_unloadUnusedChunks()
{
	size_t total = chunks.size() * sizeof(Chunk);
	
	if (total > _maxChunksMemoryUse)
	{
		typedef std::multimap<Tick, IntCoord> CoordsMap;
		CoordsMap dataCoords;
		
		for (ChunkMap::const_iterator it = chunks.begin(); it != chunks.end(); ++it)
		{
			dataCoords.insert(std::make_pair(it->second->touchTick, it->first));
		}
		
		for (CoordsMap::iterator it = dataCoords.begin(); it != dataCoords.end(); ++it)
		{
			if (total <= _maxChunksMemoryUse)
				return;
			
			if (it->first + 60 > _renderTick)
				return;
			
			_unlinkChunk(it->second);
			total -= sizeof(Chunk);
			worldProvider.save(*chunks[it->second].get(), it->second, true);
			chunks.erase(it->second);
		}
	}
}

float World::getMaxLightNearPoint(const math::vec3 &v)
{
	float res = getMaxLightAtPoint<LIGHT_R>(v);
	
	for (float i = -2.0; i <= 2.0; ++i)
	{
		for (float j = -2.0; j <= 2.0; ++j)
		{
			for (float k = -2.0; k <= 2.0; ++k)
			{
				res = std::max(res, getMaxLightAtPoint<LIGHT_SUN>(v + math::vec3(i, j, k)) * dayNightLightCoef);
				res = std::max(res, (
						getMaxLightAtPoint<LIGHT_R>(v + math::vec3(i, j, k)) +
						getMaxLightAtPoint<LIGHT_G>(v + math::vec3(i, j, k)) +
						getMaxLightAtPoint<LIGHT_B>(v + math::vec3(i, j, k))) / 3.0f
					);
			}
		}
	}

	return res;
}

template<LightType lt>
float World::getMaxLightAtPoint(const math::vec3 &v)
{
	math::ivec3 p = floorCoord(v);
	
	Chunk *chunk = getLoadedChunk(eucDivChunk(p));
	if (!chunk)
		return lt == LIGHT_SUN ? 1.0 : 0.0;

	LightValue l = chunk->lightAt<lt>(eucModChunk(p));
	return (float) l / (float) MAX_LIGHT;
}

template float World::getMaxLightAtPoint<LIGHT_SUN>(const math::vec3 &v);
template float World::getMaxLightAtPoint<LIGHT_R>(const math::vec3 &v);
template float World::getMaxLightAtPoint<LIGHT_G>(const math::vec3 &v);
template float World::getMaxLightAtPoint<LIGHT_B>(const math::vec3 &v);

Chunk *World::getChunk(const IntCoord &coord)
{
	if (!isChunkCoordValid(eucDivChunk(coord)))
		return nullptr;
	
	ChunkMap::iterator it = chunks.find(coord);
	
	if (it == chunks.end())
		return nullptr;
	
	Chunk *result = it->second.get();
	
	return result;
}

Chunk *World::getLoadedChunk(const IntCoord &coord)
{
	Chunk *result = getChunk(coord);
	
	if (!result)
		return nullptr;

	/*if (!result->isLoaded.load())
		return nullptr;*/
	
	return result;
}

CubeType World::getCubeAt(const math::ivec3 &v)
{
	Chunk *chunk = getLoadedChunk(eucDivChunk(v));
	if (!chunk)
		return 0;
	
	return chunk->cubeAt(eucModChunk(v));
}

template<LightType lt>
LightValue *World::getLightRef(const math::ivec3 &v)
{
	Chunk *chunk = getLoadedChunk(eucDivChunk(v));
	if (!chunk)
		return nullptr;

	return &chunk->rawLightRefAt<lt>(eucModChunk(v));
}

template LightValue *World::getLightRef<LIGHT_SUN>(const math::ivec3 &v);
template LightValue *World::getLightRef<LIGHT_R>(const math::ivec3 &v);
template LightValue *World::getLightRef<LIGHT_G>(const math::ivec3 &v);
template LightValue *World::getLightRef<LIGHT_B>(const math::ivec3 &v);

math::ivec3 getChunkCoord(const math::ivec3 &p)
{
	return eucDivChunk(p);
}

void adj(int i, math::ivec3 &p)
{
	switch (i)
	{
	case 0:
		p.x--;
		break;

	case 1:
		p.x++;
		break;

	case 2:
		p.y--;
		break;

	case 3:
		p.y++;
		break;

	case 4:
		p.z--;
		break;

	case 5:
		p.z++;
		break;
	}
}

struct UpdateLightSets
{
	std::vector<math::ivec3> blockSet, blockSet2;

	UpdateLightSets()
	{
		blockSet.reserve(MAX_LIGHT * MAX_LIGHT * MAX_LIGHT);
		blockSet2.reserve(MAX_LIGHT * MAX_LIGHT * MAX_LIGHT);
	}

	void clear()
	{
		blockSet.clear();
		blockSet2.clear();
	}

};

//UpdateLightSets updateLightSets;

template<LightType lt>
void World::updateLight(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks)
{
	int cnt = 0;
	int cnt2 = 0;

	std::unordered_set<math::ivec3, IntCoordHasher> lightSources;
	std::unordered_set<math::ivec3, IntCoordHasher> updatedChunks;
	UpdateLightSets updateLightSets;
	updateLightSets.clear();

	std::vector<math::ivec3> newDarkBlocks;

	for (const AddedBlockLight & l : addedBlocks)
	{
		LightValue *lp = getLightRef<lt>(l.position);

		if (!lp)
			continue;

		if (*lp > l.getLight<lt>())
		{
			newDarkBlocks.push_back(l.position);
		}

		if (l.getLight<lt>())
		{
			lightSources.insert(l.position);
		}
	}

	for (const math::ivec3 & blockCoord : removedBlocks)
	{
		LightValue *lp = getLightRef<lt>(blockCoord);

		if (lp && *lp > 0)
		{
			newDarkBlocks.push_back(blockCoord);
		}
	}

	for (const math::ivec3 & blockCoord : newDarkBlocks)
	{
		boost::timer::auto_cpu_timer t;

		LightValue *lp = getLightRef<lt>(blockCoord);

		if (!lp)
			continue;

		std::vector<math::ivec3> &blockSet = updateLightSets.blockSet, &blockSet2 = updateLightSets.blockSet2;

		LightValue l = *lp;
		*lp = 0;
		blockSet.push_back(blockCoord);

		while (l-- > 1 && !blockSet.empty())
		{
			for (const math::ivec3 & p : blockSet)
			{
				for (int i = 0; i < 6; ++i)
				{
					math::ivec3 adjP = p;
					adj(i, adjP);
					LightValue *l2 = getLightRef<lt>(adjP);

					if (l2 && *l2)
					{
						if (*l2 <= l && !getCubeAt(adjP))
						{
							//lout << "DB " << adjP.x << " " << adjP.y << " " << adjP.z << " <-- " << (int) 0 << " | " << p.x << " " << p.y << " " << p.z << std::endl;
							
							blockSet2.push_back(adjP);
							*l2 = 0;
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z + 1)));
							++cnt;
						}
						else
						{
							lightSources.insert(adjP);
							++cnt2;
						}
					}
				}
			}

			blockSet.swap(blockSet2);
			blockSet2.clear();
		}
	}

	for (const AddedBlockLight & l : addedBlocks)
	{
		LightValue *lp = getLightRef<lt>(l.position);

		if (!lp)
			continue;

		*lp = l.getLight<lt>();
		
		//lout << "AB " << l.position.x << " " << l.position.y << " " << l.position.z << " <-- " << (int) *lp << std::endl;
	}

	for (const math::ivec3 & blockCoord : removedBlocks)
	{
		LightValue *lp = getLightRef<lt>(blockCoord);

		if (lp && *lp > 0)
		{
			*lp = 0;
			
			//lout << "RB " << blockCoord.x << " " << blockCoord.y << " " << blockCoord.z << " <-- " << (int) 0 << std::endl;
		}
	}

	std::cout << "ZeroB " << cnt << " " << cnt2 << std::endl;

	for (const math::ivec3 & p : removedBlocks)
	{
		for (int i = 0; i < 6; ++i)
		{
			math::ivec3 adjP = p;
			adj(i, adjP);

			LightValue *l2 = getLightRef<lt>(adjP);

			if (l2 && *l2 > 1)
			{
				lightSources.insert(adjP);
			}
		}
	}

	std::cout << "lightSources " << lightSources.size() << std::endl;
	cnt = 0;
	boost::timer::auto_cpu_timer t;

	std::unordered_set<math::ivec3, IntCoordHasher> lightSources2;

	while (!lightSources.empty())
	{
		for (const math::ivec3 & p : lightSources)
		{
			LightValue l1 = *getLightRef<lt>(p);

			if (l1-- > 1)
			{
				for (int i = 0; i < 6; ++i)
				{
					math::ivec3 adjP = p;
					adj(i, adjP);

					LightValue *l2 = getLightRef<lt>(adjP);

					if (l2 && *l2 < l1 && !getCubeAt(adjP))
					{
						//lout << "LS " << adjP.x << " " << adjP.y << " " << adjP.z << " <-- " << (int) l1 << " | " << p.x << " " << p.y << " " << p.z << std::endl;
						
						*l2 = l1;
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z - 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z + 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z - 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z + 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z - 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z + 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z - 1)));
						updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z + 1)));
						lightSources2.insert(adjP);
						++cnt;
					}
				}
			}
		}

		lightSources.swap(lightSources2);
		lightSources2.clear();
	}

	std::cout << "UpdB " << cnt << std::endl;

	for (const math::ivec3 & p : updatedChunks)
	{
		Chunk *chunk = getLoadedChunk(p);
		if (!chunk)
			continue;

		chunk->isDirty = true;
	}
}

template void World::updateLight<LIGHT_R>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_G>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_B>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_SUN>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);

bool World::getBlock(const math::vec3 &p, const math::vec3 &ray, float len, math::ivec3 &result, math::ivec3 &prev)
{
	const float STEP = 0.01f;

	math::vec3 s = p + math::vec3(0.5f, 0.5f, 0.5f);
	math::vec3 d = ray * STEP;


	math::ivec3 bp = floorCoord(p);

	for (; (len -= STEP) > 0.0f;)
	{
		s += d;
		math::ivec3 bp_new = floorCoord(s);

		if (bp != bp_new)
		{
			CubeType cube = getCubeAt(bp_new);

			if (cube != 0)
			{
				result = bp_new;
				prev = bp;
				return true;
			}

			bp = bp_new;
		}
	}

	return false;
}

bool World::putBlock(const math::ivec3 &v, CubeType c)
{
	math::ivec3 chuckC = getChunkCoord(v);
	
	Chunk *chunk = getLoadedChunk(chuckC);
	if (!chunk)
		return false;

	math::ivec3 bp = eucModChunk(v);
	chunk->put(bp, c);
	chunk->isDirty = true;

	/*if (chuckC.x > 1)
		chunks[boost::make_tuple(chuckC.x - 1, chuckC.y, chuckC.z)]->isDirty = true;
	if (chuckC.x < S - 1)
		chunks[boost::make_tuple(chuckC.x + 1, chuckC.y, chuckC.z)]->isDirty = true;
	if (chuckC.y > 1)
		chunks[boost::make_tuple(chuckC.x, chuckC.y - 1, chuckC.z)]->isDirty = true;
	if (chuckC.y < S - 1)
		chunks[boost::make_tuple(chuckC.x, chuckC.y + 1, chuckC.z)]->isDirty = true;
	if (chuckC.z > 1)
		chunks[boost::make_tuple(chuckC.x, chuckC.y, chuckC.z - 1)]->isDirty = true;
	if (chuckC.z < SZ - 1)
		chunks[boost::make_tuple(chuckC.x, chuckC.y, chuckC.z + 1)]->isDirty = true;*/
	return true;
}

void World::move(math::BBox &box, const math::vec3 &delta)
{
	float len = math::length(delta);

	if (len <= 0.0f)
		return;

	int iterN = (int)(len * 2.0f + 1.0);
	math::vec3 d = delta / (float) iterN;

	for (float i = 0; i < iterN; ++i)
	{
		math::vec3 p1 = math::floor(box.point - math::vec3(1.0f, 1.0f, 1.0f) + d);
		math::vec3 p2 = math::floor(box.point + box.size + math::vec3(1.0f, 1.0f, 1.0f) + d);

		float dd = d.x;

		for (int x = p1.x; x <= p2.x; ++x)
		{
			for (int y = p1.y; y <= p2.y; ++y)
			{
				for (int z = p1.z; z <= p2.z; ++z)
				{
					if (getCubeAt(math::ivec3(x, y, z)))
					{
						math::BBox cb(math::vec3(x, y, z) - math::vec3(0.5f, 0.5f, 0.5f), math::vec3(1.0f, 1.0f, 1.0f));
						dd = cb.calculateXOffset(box, dd);
					}
				}
			}
		}

		box.point.x += dd;

		dd = d.y;

		for (int x = p1.x; x <= p2.x; ++x)
		{
			for (int y = p1.y; y <= p2.y; ++y)
			{
				for (int z = p1.z; z <= p2.z; ++z)
				{
					if (getCubeAt(math::ivec3(x, y, z)))
					{
						math::BBox cb(math::vec3(x, y, z) - math::vec3(0.5f, 0.5f, 0.5f), math::vec3(1.0f, 1.0f, 1.0f));
						dd = cb.calculateYOffset(box, dd);
					}
				}
			}
		}

		box.point.y += dd;

		dd = d.z;

		for (int x = p1.x; x <= p2.x; ++x)
		{
			for (int y = p1.y; y <= p2.y; ++y)
			{
				for (int z = p1.z; z <= p2.z; ++z)
				{
					if (getCubeAt(math::ivec3(x, y, z)))
					{
						math::BBox cb(math::vec3(x, y, z) - math::vec3(0.5f, 0.5f, 0.5f), math::vec3(1.0f, 1.0f, 1.0f));
						dd = cb.calculateZOffset(box, dd);
					}
				}
			}
		}

		box.point.z += dd;
	}
}

void World::create()
{
	dayNightLightCoef = 1.0f;
	solidChunk.reset(new Chunk);
	Chunk *pSolidChunk = solidChunk.get();
	pSolidChunk->isDummy = true;
	pSolidChunk->l = pSolidChunk->r = pSolidChunk->u = pSolidChunk->d = pSolidChunk->b = pSolidChunk->f = pSolidChunk;

	std::cout << "Fill solidChunk" << std::endl;

	for (unsigned int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (unsigned int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (unsigned int k = 0; k < CHUNK_SIZE; ++k)
			{
				solidChunk->put(math::ivec3(i, j, k), 1);
			}
		}
	}

	/*std::cout << "create data" << std::endl;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				chunks[IntCoord(i, j, k)].reset(new Chunk);
				renderData[IntCoord(i, j, k)].reset(new RenderData);
			}
		}
	}

	std::cout << "Link data" << std::endl;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				Chunk &chunk = *chunks[IntCoord(i, j, k)];

				chunk.l = (i == 0)       ? pSolidChunk : chunks[IntCoord(i - 1, j, k)].get();
				chunk.r = (i == (S - 1)) ? pSolidChunk : chunks[IntCoord(i + 1, j, k)].get();

				chunk.b = (j == 0)       ? pSolidChunk : chunks[IntCoord(i, j - 1, k)].get();
				chunk.f = (j == (S - 1)) ? pSolidChunk : chunks[IntCoord(i, j + 1, k)].get();

				chunk.d = (k == 0)       ? pSolidChunk : chunks[IntCoord(i, j, k - 1)].get();
				chunk.u = (k == (SZ - 1)) ? pSolidChunk : chunks[IntCoord(i, j, k + 1)].get();

				std::cout << i << " " << j << " " << k << " " << chunk.d->isDummy << std::endl;
			}
		}
	}


	std::cout << "Fill" << std::endl;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				Chunk &chunk = *chunks[IntCoord(i, j, k)];

				if (worldProvider.load(chunk, math::ivec3(i, j, k)))
					continue;

				worldProvider.fill(chunk, math::ivec3(i, j, k));
			}
		}
	}

	std::cout << "updateSunLight" << std::endl;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				Chunk &chunk = *chunks[IntCoord(i, j, k)];
				chunk.updateSunLight();
				chunk.updateLight();
			}
		}
	}

	for (int l = 0; l < MAX_LIGHT; ++l)
	{
		std::cout << "updateLightIter " << l << std::endl;
		int updCount = 0;

		for (int i = 0; i < S; ++i)
		{
			for (int j = 0; j < S; ++j)
			{
				for (int k = 0; k < SZ; ++k)
				{
					Chunk &chunk = *chunks[IntCoord(i, j, k)];
					updCount += chunk.updateSunLightIter();
				}
			}
		}

		if (!updCount)
			break;
	}

	std::cout << "saveData" << std::endl;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				Chunk &chunk = *chunks[IntCoord(i, j, k)];
				worldProvider.save(chunk, math::ivec3(i, j, k), false);
			}
		}
	}

	std::cout << "uploadData" << std::endl;
	size_t videoMemUse = 0;

	for (int i = 0; i < S; ++i)
	{
		for (int j = 0; j < S; ++j)
		{
			for (int k = 0; k < SZ; ++k)
			{
				Chunk &chunk = *chunks[IntCoord(i, j, k)];
				RenderData &rd = *renderData[IntCoord(i, j, k)];
				rd.addVertexData(chunk);
				rd.uploadData();
				videoMemUse += rd.getVideoMemUse();
			}
		}
	}

	std::cout << "videoMemUse " << videoMemUse << std::endl;
*/
	
	/////////////////////////
	if (oqVao == 0)
		glGenVertexArrays(1, &oqVao);

	glBindVertexArray(oqVao);

	if (oqTriangleBufferObject == 0)
		glGenBuffers(1, &oqTriangleBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, oqTriangleBufferObject); //we're "using" this one now
	float oqVertices[] =
	{
		0, 0, 0,
		CHUNK_SIZE, 0, 0,
		0, CHUNK_SIZE, 0,
		CHUNK_SIZE, CHUNK_SIZE, 0,
		0, 0, CHUNK_SIZE,
		CHUNK_SIZE, 0, CHUNK_SIZE,
		0, CHUNK_SIZE, CHUNK_SIZE,
		CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(oqVertices), oqVertices, GL_STATIC_DRAW); //formatting the data for the buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

	if (oqTriangleIndicesBufferObject == 0)
		glGenBuffers(1, &oqTriangleIndicesBufferObject); //create the buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oqTriangleIndicesBufferObject); //we're "using" this one now

	int oqIndeces[] =
	{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
		0, 1, 4,
		1, 4, 5,
		2, 3, 6,
		3, 6, 7,
		0, 2, 4,
		2, 4, 6,
		1, 3, 5,
		3, 5, 7
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(oqIndeces), oqIndeces, GL_STATIC_DRAW); //formatting the data for the buffer

	glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0); //tell gl (shader!) how to interpret our vertex data
	glBindVertexArray(0);

	printf("OQ glError: %d\n", glGetError());

	oqShader.buildShaderProgram("oqvs.glsl", "oqfs.glsl");
	////////////////////////////////////////

	worldShader.buildShaderProgram("wvs.glsl", "wgs.glsl", "wfs.glsl");
}

void World::_linkChunk(Chunk *chunk, const IntCoord &pos)
{
	Chunk *pSolidChunk = solidChunk.get();
	
	Chunk *ch = getChunk(IntCoord(pos.x - 1, pos.y, pos.z));
	if (ch)
	{
		chunk->l = ch;
		ch->r = chunk;
	}
	else
	{
		chunk->l = pSolidChunk;
	}
	
	ch = getChunk(IntCoord(pos.x + 1, pos.y, pos.z));
	if (ch)
	{
		chunk->r = ch;
		ch->l = chunk;
	}
	else
	{
		chunk->r = pSolidChunk;
	}
	
	ch = getChunk(IntCoord(pos.x, pos.y - 1, pos.z));
	if (ch)
	{
		chunk->b = ch;
		ch->f = chunk;
	}
	else
	{
		chunk->b = pSolidChunk;
	}
	
	ch = getChunk(IntCoord(pos.x, pos.y + 1, pos.z));
	if (ch)
	{
		chunk->f = ch;
		ch->b = chunk;
	}
	else
	{
		chunk->f = pSolidChunk;
	}
	
	ch = getChunk(IntCoord(pos.x, pos.y, pos.z - 1));
	if (ch)
	{
		chunk->d = ch;
		ch->u = chunk;
	}
	else
	{
		chunk->d = pSolidChunk;
	}
	
	ch = getChunk(IntCoord(pos.x, pos.y, pos.z + 1));
	if (ch)
	{
		chunk->u = ch;
		ch->d = chunk;
	}
	else
	{
		chunk->u = pSolidChunk;
	}
}

void World::_unlinkChunk(const IntCoord &pos)
{
	Chunk *pSolidChunk = solidChunk.get();
	
	Chunk *ch = getChunk(IntCoord(pos.x - 1, pos.y, pos.z));
	if (ch && !ch->isDummy)
		ch->r = pSolidChunk;
	
	ch = getChunk(IntCoord(pos.x + 1, pos.y, pos.z));
	if (ch && !ch->isDummy)
		ch->l = pSolidChunk;
	
	ch = getChunk(IntCoord(pos.x, pos.y - 1, pos.z));
	if (ch && !ch->isDummy)
		ch->f = pSolidChunk;
	
	ch = getChunk(IntCoord(pos.x, pos.y + 1, pos.z));
	if (ch && !ch->isDummy)
		ch->b = pSolidChunk;
	
	ch = getChunk(IntCoord(pos.x, pos.y, pos.z - 1));
	if (ch && !ch->isDummy)
		ch->u = pSolidChunk;
	
	ch = getChunk(IntCoord(pos.x, pos.y, pos.z + 1));
	if (ch && !ch->isDummy)
		ch->d = pSolidChunk;
}

bool World::isChunkCanBeRendered(const IntCoord &coord) const
{
	//return chunks.count(coord);
	
	if (!isChunkCoordValid(coord))
		return false;
	
	ChunkMap::const_iterator it;
	
	it = chunks.find(coord);
	if (it == chunks.end() || !it->second->isLighted.load() || !it->second->blockCount)
		return false;
	
	for (int x = -1; x <= 1 ; ++x)
	{
		for (int y = -1; y <= 1 ; ++y)
		{
			for (int z = -1; z <= 1 ; ++z)
			{
				if (x != 0 || y != 0 || z != 0)
				{
					IntCoord c = coord + IntCoord(x, y, z);
					it = chunks.find(c);
					if (isChunkCoordValid(c) && (it == chunks.end() || !it->second->isLighted.load()))
						return false;
				}
			}
		}
	}
	
	return true;
}

int dirty1 = 0, dirty2 = 0;

void World::render(const Camera &camera, const cyberCubes::Player &player)
{
	++_renderTick;
	
	math::mat4 MVP = camera.getVP();
	math::Frustum frustum;
	frustum.update(MVP);

	typedef std::multimap<float, IntCoord> VisibleChunksMap;
	VisibleChunksMap visibleChunks;

	for (IntCoord coord : player.getChunks())
	{
		Chunk *chunk;

		if (chunks.count(coord))
		{
			chunk = chunks[coord].get();
		}
		else
		{
			if (!pendingChunks.count(coord))
			{
				pendingChunks[coord].reset(chunk = new Chunk);
				ChunkLoadRequest request;
				request.chunkCoord = coord;
				request.chunk = chunk;
				_pendingChunkCoords.push(request);
			}
			else
			{
				chunk = pendingChunks[coord].get();

				if (chunk->isLoaded.load())
				{
					chunks.emplace(std::make_pair(coord, std::move(pendingChunks[coord])));
					pendingChunks.erase(coord);
					_linkChunk(chunk, coord);
					
					ChunkLightUpRequest request;
					request.chunkCoord = coord;
					request.isSunLight = false;
					request.chunk = chunk;
					_lightingChunkCoords.push(request);
				}
			}
		}
		
		chunk->touchTick = _renderTick;
		
		if (isChunkCanBeRendered(coord))
		{
			math::vec3 pos = i2f(coord) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);
			math::BBox bbox(pos, math::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));

			if (frustum.checkBBox(bbox) != math::IntersectionType::OUTSIDE)
			{
				pos += math::vec3(CHUNK_SIZE / 2, CHUNK_SIZE / 2, CHUNK_SIZE / 2) - camera.position;
				float dist2 = math::dot(pos, pos);
				visibleChunks.insert(std::make_pair(dist2, coord));
			}
		}
	}

	std::vector<IntCoord> visibleChunksVector;
	std::vector<bool> oqVisibleChunksVector;

	for (VisibleChunksMap::iterator it = visibleChunks.begin(); it != visibleChunks.end(); ++it)
		visibleChunksVector.push_back(it->second);

	//std::cout << "SZ " << visibleChunksVector.size() << std::endl;

	int trisRendred = 0;
	int occluded = 0;

	size_t l = 2;

	for (size_t i = 0; i < l && i < visibleChunksVector.size(); ++i)
	{
		oqVisibleChunksVector.push_back(true);
	}

	for (size_t n = 0; n < visibleChunksVector.size();)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glUseProgram(worldShader.program);

		if (worldShader.uniforms.count("MVP"))
			glUniformMatrix4fv(worldShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);

		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			if (oqVisibleChunksVector[i])
			{
				IntCoord &c = visibleChunksVector[i];
				math::vec3 pos = i2f(c) * CHUNK_SIZE_F;
				glUniform3fv(worldShader.uniforms["chunkPosition"], 1, &pos[0]);
				glUniform1f(worldShader.uniforms["dayNightLightCoef"], dayNightLightCoef);

				Chunk &chunk = *chunks[c];
				
				if (!renderData.count(c))
				{
					renderData[c].reset(new RenderData);
					chunk.isDirty = true;
					
					dirty1++;
				}
				
				RenderData &rd = *renderData[c];

				if (chunk.isDirty)
				{
					chunk.isDirty = false;
					rd.addVertexData(chunk);
					rd.uploadData();
					
					dirty2++;
				}
				
				rd.render(worldShader.uniforms["norm"], worldShader.uniforms["t1"], worldShader.uniforms["t2"], camera.position - pos, _renderTick);
				trisRendred += rd.trisRendered;
			}
			else
			{
				occluded++;
			}
		}

		glUseProgram(0);

		glUseProgram(oqShader.program);

		if (oqShader.uniforms.count("MVP"))
			glUniformMatrix4fv(oqShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(oqVao);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		n += l;
		l *= 2;

		std::unordered_map<IntCoord, GLuint, IntCoordHasher> oqIds;
		
		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			IntCoord &c = visibleChunksVector[i];

			GLuint queryId = oqIds[c] = queryPool.getQueryId();
			glBeginQuery(GL_ANY_SAMPLES_PASSED, queryId);
			//std::cout << "Query " << c.get<0>() << " " << c.get<1>() << " " << c.get<2>() << " " << oqIds[c] << std::endl;

			math::vec3 pos = i2f(c) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);

			glUniform3fv(oqShader.uniforms["chunkPosition"], 1, &pos[0]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

			glEndQuery(GL_ANY_SAMPLES_PASSED);
		}

		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			IntCoord &c = visibleChunksVector[i];
			GLuint params;
			GLuint queryId = oqIds[c];
			glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &params);
			queryPool.releaseQueryId(queryId);
			oqVisibleChunksVector.push_back(params == 1);
		}

		//glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	//std::cout << "triangles " << trisRendred << " OQ " << occluded  << "/" << visibleChunksVector.size() << " memUse: " << _getTotalRenderDataMemoryUse()/1024.0/1024.0 << " MB" << std::endl;
	//std::cout << "Chunk Mem " << chunks.size()*sizeof(Chunk)/1024.0/1024.0 << " MB, " << pendingChunks.size()*sizeof(Chunk)/1024.0/1024.0 << " MB" << std::endl;
	
	std::cout << dirty1 << " " << dirty2 << std::endl;
	
	releaseOldRenderData();
	_unloadUnusedChunks();
}

size_t World::_getTotalRenderDataMemoryUse() const
{
	size_t result = 0;
	
	for (RenderDataMap::const_iterator it = renderData.begin(); it != renderData.end(); ++it)
	{
		result += it->second->getVideoMemUse();
	}
	
	return result;
}

void World::releaseOldRenderData()
{
	size_t total = _getTotalRenderDataMemoryUse();
	
	if (total > _maxRenderDataMemoryUse)
	{
		typedef std::multimap<Tick, IntCoord> CoordsMap;
		CoordsMap dataCoords;
		
		for (RenderDataMap::const_iterator it = renderData.begin(); it != renderData.end(); ++it)
		{
			dataCoords.insert(std::make_pair(it->second->getLastRenderTick(), it->first));
		}
		
		for (CoordsMap::iterator it = dataCoords.begin(); it != dataCoords.end(); ++it)
		{
			if (total <= _maxRenderDataMemoryUse)
				return;
			
			total -= renderData[it->second]->getVideoMemUse();
			renderData.erase(it->second);
		}
	}
}
