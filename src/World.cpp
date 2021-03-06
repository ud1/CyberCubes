#include "World.hpp"
#include "Chunk.hpp"
#include "RenderData.hpp"
#include "Frustum.hpp"
#include <iostream>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>
#include <cmath>
#include <libnoise/noise.h>
#include <boost/timer/timer.hpp>
#include "Block.hpp"
#include "blockType.hpp"

#include <fstream>

#include <chrono>
#include <thread>

#include "GeomOcclusitionCulling.hpp"

const int S = 4;
const int SZ = 16;

//std::ofstream lout("lout.txt", std::ofstream::out | std::ofstream::trunc);

void checkGLError(const char *str);

struct WorldProviderTransaction
{
	WorldProviderTransaction(WorldProvider &provider) : provider(provider)
	{
		provider.beginTransaction();
	}
	
	~WorldProviderTransaction()
	{
		provider.endTransaction();
	}
	
	WorldProvider &provider;
};

World::World() : oqShader("oqShader"), worldShaderPass1("worldShaderPass1"), worldShaderPass2("worldShaderPass2"), shadowMapPass("shadowMapShader"),
modelTextures(math::ivec2(256, 256)), modelRenderer(modelTextures)
{
	_renderTick = 0;
	_maxRenderDataMemoryUse = 700 * 1024 * 1024;
	_maxChunksMemoryUse = 5000L * 1024L * 1024L;
	
	constexpr int W = 100000;
	minWorldChunkCoord = IntCoord(-W, -W, -W);
	maxWorldChunkCoord = IntCoord(W, W, SZ);
	
	_isLoadingThreadRunned.test_and_set();
	_loadChunksThread.reset(new std::thread(&World::_loadChunks, this));
	
	_isLightingThreadRunned.test_and_set();
	_lightingChunksThread.reset(new std::thread(&World::_lightChunks, this));
	
	_isLoadSLLayerThreadRunned.test_and_set();
	_loadSLLayerThread.reset(new std::thread(&World::_loadSLLayers, this));
}

World::~World()
{
	_isLoadingThreadRunned.clear();
	_pendingChunkCoords.push(ChunkLoadRequest());
	
	_isLightingThreadRunned.clear();
	_lightingChunkCoords.push(ChunkLightUpRequest());
	
	_isLoadSLLayerThreadRunned.clear();
	_slLayerCoords.push(SLLayerLoadRequest());
	
	_loadChunksThread->join();
	_lightingChunksThread->join();
	_loadSLLayerThread->join();
	
	WorldProviderTransaction transaction(worldProvider);
	for (ChunkMap::const_iterator it = chunks.begin(); it != chunks.end(); ++it)
	{
		if (it->second.get()->needToPersist)
			worldProvider.save(*it->second.get(), it->first, true);
		
		SunLightPropagationLayer &slpl = sunLightPropagationLayerMap[it->first];
		if (slpl.needToPersist)
			worldProvider.save(slpl, it->first);
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
		math::ivec3 chunkPos = request.chunkCoord * CHUNK_SIZE_I;
		request.chunk->createObjects(chunkPos, modelTextures);
	}
	_isLoadingThreadRunned.clear();
}

void World::_loadSLLayers()
{
	while(true)
	{
		SLLayerLoadRequest request = _slLayerCoords.pop();
		
		if (!_isLoadSLLayerThreadRunned.test_and_set())
			break;
		
		bool res = worldProvider.load(*request.layer, request.chunkCoord);
		
		if (res)
			request.layer->isLoaded.store(true);
		else
			request.layer->isCannotBeLoaded.store(true);
	}
	_isLoadSLLayerThreadRunned.clear();
}

void updateBlockLightVector(Chunk *chunk, const math::ivec3 &p, const math::ivec3 &chunkPos,
			    std::vector<AddedBlockLight> &addedBlocks, bool sun)
{
	if (sun)
	{
		if (chunk->rawLightRefAt<LIGHT_SUN>(p) > 0)
		{
			AddedBlockLight lb;
			lb.position = p + chunkPos;
			lb.lightColor[LIGHT_SUN] = chunk->rawLightRefAt<LIGHT_SUN>(p);
			addedBlocks.push_back(lb);
		}
	}
	else
	{
		CubeType cube = chunk->rawCubeAt(p);

		if (cube)
		{
			const Block *block = Block::get(cube);

			if (block)
			{
				if (block->lightValueR)
					chunk->rawLightRefAt<LIGHT_R>(p) = block->lightValueR;
				if (block->lightValueG)
					chunk->rawLightRefAt<LIGHT_G>(p) = block->lightValueG;
				if (block->lightValueB)
					chunk->rawLightRefAt<LIGHT_B>(p) = block->lightValueB;
			}
		}
		
		if (chunk->rawLightRefAt<LIGHT_R>(p) > 0 || chunk->rawLightRefAt<LIGHT_G>(p) > 0 || chunk->rawLightRefAt<LIGHT_B>(p) > 0)
		{
			AddedBlockLight lb;
			lb.position = p + chunkPos;
			lb.lightColor[LIGHT_R] = chunk->rawLightRefAt<LIGHT_R>(p);
			lb.lightColor[LIGHT_G] = chunk->rawLightRefAt<LIGHT_G>(p);
			lb.lightColor[LIGHT_B] = chunk->rawLightRefAt<LIGHT_B>(p);
			addedBlocks.push_back(lb);
		}
	}
}

LightValue sunLightAt(int z)
{
	if (z < 0)
		return 0;
	if (z >= 256)
		return MAX_LIGHT;
	
	return z / 8;
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

struct ChunkPtrMapAccessor
{
	ChunkPtrMapAccessor(World::ChunkPtrMap &map) : map(map) {}
	World::ChunkPtrMap &map;

	template<LightType lt>
	LightValue *getLightRef(const math::ivec3 &v)
	{
		return &getChunk(eucDivChunk(v))->rawLightRefAt<lt>(eucModChunk(v));
	}

	CubeType getCubeAt(const math::ivec3 &v)
	{
		return getChunk(eucDivChunk(v))->rawCubeAt(eucModChunk(v));
	}

	Chunk *getChunk(const IntCoord &coord)
	{
		return map[coord];
	}
};

template<LightType lt, typename Accessor, bool updateDirtyFlags>
void updateLight(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks, Accessor &a);

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

		if (!request.isSunLight)
		{
			for (int x = 0; x < CHUNK_SIZE; ++x)
			{
				for (int y = 0; y < CHUNK_SIZE; ++y)
				{
					for (int z = 0; z < CHUNK_SIZE; ++z)
					{
						updateBlockLightVector(request.chunk, math::ivec3(x, y, z), request.chunkCoord * CHUNK_SIZE_I, addedBlocks, false);
					}
				}
			}
		}
		else
		{
			SunLightPropagationSum *sum = request.sunLightPropagationSum.get();
			Chunk *chunk = request.chunk;
			IntCoord chunkPos = request.chunkCoord * CHUNK_SIZE_I;
			for (int x = 0; x < CHUNK_SIZE; ++x)
			{
				for (int y = 0; y < CHUNK_SIZE; ++y)
				{
					int k = 0;
					if (sum->valueAt(x, y) == 0)
					{
						for (int z = CHUNK_SIZE; z -- > 0;)
						{
							IntCoord p = IntCoord(x, y, z);
							if (!isSunFullyTransparent(chunk->rawCubeAt(p)))
								break;
							
							LightValue lv = sunLightAt(z + chunkPos.z);
							chunk->rawLightRefAt<LIGHT_SUN>(p) = lv;
						}
					}
				}
			}
			
			for (int x = 0; x < CHUNK_SIZE; ++x)
			{
				for (int y = 0; y < CHUNK_SIZE; ++y)
				{
					int k = 0;
					if (sum->valueAt(x, y) == 0)
					{
						for (int z = CHUNK_SIZE; z -- > 0;)
						{
							IntCoord p = IntCoord(x, y, z);
							if (!isSunFullyTransparent(chunk->rawCubeAt(p)))
								break;
							
							bool addLightSource = false;
							if (x > 0 && x < CHUNK_SIZE_M1_I && y > 0 && y < CHUNK_SIZE_M1_I && z > 0 && z < CHUNK_SIZE_M1_I)
							{
								for (int i = 0; i < 6; ++i)
								{
									math::ivec3 adjP = p;
									adj(i, adjP);
									LightValue lv = sunLightAt(adjP.z + chunkPos.z);
									LightValue bl = chunk->rawLightRefAt<LIGHT_SUN>(adjP);
									if (lv != bl)
									{
										addLightSource = true;
										break;
									}
								}
							}
							else
							{
								addLightSource = true;
							}
						
							if (addLightSource)
							{
								LightValue lv = sunLightAt(z + chunkPos.z);
								if (lv > 0)
								{
									AddedBlockLight lb;
									lb.position = p + chunkPos;
									lb.lightColor[LIGHT_SUN] = lv;
									addedBlocks.push_back(lb);
									++k;
								}
							}
						}
					}
				}
			}
			//lout << "SL " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		}

		//lout << "REQS2 " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				math::ivec3 chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.z -= CHUNK_SIZE_I;

				if (!request.chunk->d->isDummy)
					updateBlockLightVector(request.chunk->d, math::ivec3(x, y, CHUNK_SIZE_I - 1), chunkPos, addedBlocks, request.isSunLight);

				chunkPos.z += 2 * CHUNK_SIZE_I;

				if (!request.chunk->u->isDummy)
					updateBlockLightVector(request.chunk->u, math::ivec3(x, y, 0), chunkPos, addedBlocks, request.isSunLight);

				chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.x -= CHUNK_SIZE_I;

				if (!request.chunk->l->isDummy)
					updateBlockLightVector(request.chunk->l, math::ivec3(CHUNK_SIZE_I - 1, x, y), chunkPos, addedBlocks, request.isSunLight);

				chunkPos.x += 2 * CHUNK_SIZE_I;

				if (!request.chunk->r->isDummy)
					updateBlockLightVector(request.chunk->r, math::ivec3(0, x, y), chunkPos, addedBlocks, request.isSunLight);


				chunkPos = request.chunkCoord * CHUNK_SIZE_I;
				chunkPos.y -= CHUNK_SIZE_I;

				if (!request.chunk->b->isDummy)
					updateBlockLightVector(request.chunk->b, math::ivec3(x, CHUNK_SIZE_I - 1, y), chunkPos, addedBlocks, request.isSunLight);

				chunkPos.y += 2 * CHUNK_SIZE_I;

				if (!request.chunk->f->isDummy)
					updateBlockLightVector(request.chunk->f, math::ivec3(x, 0, y), chunkPos, addedBlocks, request.isSunLight);
			}
		}

		//lout << "UPD " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;

		if (!addedBlocks.empty())
		{
			ChunkPtrMapAccessor accessor(request.chunkPtrMap);
			
			if (request.isSunLight)
			{
				::updateLight<LIGHT_SUN, ChunkPtrMapAccessor, false>(addedBlocks, removedBlocks, accessor);
			}
			else
			{
				::updateLight<LIGHT_R, ChunkPtrMapAccessor, false>(addedBlocks, removedBlocks, accessor);
				::updateLight<LIGHT_G, ChunkPtrMapAccessor, false>(addedBlocks, removedBlocks, accessor);
				::updateLight<LIGHT_B, ChunkPtrMapAccessor, false>(addedBlocks, removedBlocks, accessor);
			}
		}
		
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				for (int z = -1; z <= 1; ++z)
				{
					IntCoord p = request.chunkCoord + IntCoord(x, y, z);
					if (!request.chunkPtrMap[p]->isDummy)
					{
						request.chunkPtrMap[p]->isDirty = true;
					}
				}	
			}	
		}
		
		if (request.isSunLight)
		{
			request.chunk->isSunLighted.store(true);
		}
		else
		{
			request.chunk->isLighted.store(true);
		}

		//lout << "REQF " << request.chunkCoord.x << " " << request.chunkCoord.y << " " << request.chunkCoord.z << std::endl;
		
		
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
		
		WorldProviderTransaction transaction(worldProvider);
		
		for (CoordsMap::iterator it = dataCoords.begin(); it != dataCoords.end(); ++it)
		{
			if (total <= _maxChunksMemoryUse)
				return;
			
			if (it->first + 60 > _renderTick)
				return;
			
			Chunk *ch = getChunk(it->second);
			if (!ch->isLighted.load() || ch->sunLightRecalculating && !ch->isSunLighted.load())
				continue;
			
			_unlinkChunk(it->second);
			total -= sizeof(Chunk);
			if (chunks[it->second].get()->needToPersist)
				worldProvider.save(*chunks[it->second].get(), it->second, true);
			
			SunLightPropagationLayer &slpl = sunLightPropagationLayerMap[it->second];
			if (slpl.needToPersist)
			{
				worldProvider.save(sunLightPropagationLayerMap[it->second], it->second);
				slpl.needToPersist = false;
			}
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
	
	Chunk *chunk = getChunk(eucDivChunk(p));
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

CubeType World::getCubeAt(const math::ivec3 &v)
{
	Chunk *chunk = getChunk(eucDivChunk(v));
	if (!chunk)
		return 0;
	
	return chunk->rawCubeAt(eucModChunk(v));
}

template<LightType lt>
LightValue *World::getLightRef(const math::ivec3 &v)
{
	Chunk *chunk = getChunk(eucDivChunk(v));
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


template<LightType lt, typename Accessor, bool updateDirtyFlags>
void updateLight(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks, Accessor &a)
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
		LightValue *lp = a.template getLightRef<lt>(l.position);

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
		LightValue *lp = a.template getLightRef<lt>(blockCoord);

		if (lp && *lp > 0)
		{
			newDarkBlocks.push_back(blockCoord);
		}
	}

	for (const math::ivec3 & blockCoord : newDarkBlocks)
	{
		//boost::timer::auto_cpu_timer t;

		LightValue *lp = a.template getLightRef<lt>(blockCoord);

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
					LightValue *l2 = a.template getLightRef<lt>(adjP);

					if (l2 && *l2)
					{
						if (*l2 <= l && isLightTransparent(a.getCubeAt(adjP)))
						{
							//lout << "DB " << adjP.x << " " << adjP.y << " " << adjP.z << " <-- " << (int) 0 << " | " << p.x << " " << p.y << " " << p.z << std::endl;
							
							blockSet2.push_back(adjP);
							*l2 = 0;
							
							if (updateDirtyFlags)
							{
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z - 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z + 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z - 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z + 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z - 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z + 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z - 1)));
								updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z + 1)));
							}
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
		LightValue *lp = a.template getLightRef<lt>(l.position);

		if (!lp)
			continue;

		*lp = l.getLight<lt>();
		
		//lout << "AB " << l.position.x << " " << l.position.y << " " << l.position.z << " <-- " << (int) *lp << std::endl;
	}

	for (const math::ivec3 & blockCoord : removedBlocks)
	{
		LightValue *lp = a.template getLightRef<lt>(blockCoord);

		if (lp && *lp > 0)
		{
			*lp = 0;
			
			//lout << "RB " << blockCoord.x << " " << blockCoord.y << " " << blockCoord.z << " <-- " << (int) 0 << std::endl;
		}
	}

	//std::cout << "ZeroB " << cnt << " " << cnt2 << std::endl;

	for (const math::ivec3 & p : removedBlocks)
	{
		for (int i = 0; i < 6; ++i)
		{
			math::ivec3 adjP = p;
			adj(i, adjP);

			LightValue *l2 = a.template getLightRef<lt>(adjP);

			if (l2 && *l2 > 1)
			{
				lightSources.insert(adjP);
			}
		}
	}

	//std::cout << "lightSources " << lightSources.size() << std::endl;
	cnt = 0;
	//boost::timer::auto_cpu_timer t;

	std::unordered_set<math::ivec3, IntCoordHasher> lightSources2;

	while (!lightSources.empty())
	{
		for (const math::ivec3 & p : lightSources)
		{
			LightValue l1 = *a.template getLightRef<lt>(p);

			if (l1-- > 1)
			{
				for (int i = 0; i < 6; ++i)
				{
					math::ivec3 adjP = p;
					adj(i, adjP);

					LightValue *l2 = a.template getLightRef<lt>(adjP);

					if (l2 && *l2 < l1 && isLightTransparent(a.getCubeAt(adjP)))
					{
						//lout << "LS " << adjP.x << " " << adjP.y << " " << adjP.z << " <-- " << (int) l1 << " | " << p.x << " " << p.y << " " << p.z << std::endl;
						
						*l2 = l1;
						if (updateDirtyFlags)
						{
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y - 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x - 1, adjP.y + 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y - 1, adjP.z + 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z - 1)));
							updatedChunks.insert(getChunkCoord(math::ivec3(adjP.x + 1, adjP.y + 1, adjP.z + 1)));
						}
						lightSources2.insert(adjP);
						++cnt;
					}
				}
			}
		}

		lightSources.swap(lightSources2);
		lightSources2.clear();
	}

	//std::cout << "UpdB " << cnt << std::endl;

	if (updateDirtyFlags)
	{
		for (const math::ivec3 & p : updatedChunks)
		{
			Chunk *chunk = a.getChunk(p);
			if (!chunk)
				continue;

			chunk->isDirty = true;
		}
	}
}

template<LightType lt>
void World::updateLight(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks)
{
	Accessor accessor(this);
	::updateLight<lt, Accessor, true>(addedBlocks, removedBlocks, accessor);
}

template void World::updateLight<LIGHT_R>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_G>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_B>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
template void World::updateLight<LIGHT_SUN>(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);

bool World::getBlock(const math::vec3 &p, const math::vec3 &ray, float len, math::ivec3 &result, math::ivec3 &prev)
{
	const float STEP = 0.01f;

	math::vec3 s = p;
	
	math::ivec3 bp_new = floorCoord(p + math::vec3(0.5f, 0.5f, 0.5f));
	math::ivec3 bp;
	
	for (;;)
	{
		bp = bp_new;
		
		double xs = ray.x > 0.0 ? (bp.x + 0.5) : (bp.x - 0.5);
		double ys = ray.y > 0.0 ? (bp.y + 0.5) : (bp.y - 0.5);
		double zs = ray.z > 0.0 ? (bp.z + 0.5) : (bp.z - 0.5);
		
		double xd = std::abs(xs - s.x);
		double yd = std::abs(ys - s.y);
		double zd = std::abs(zs - s.z);
		
		double xt = std::abs(ray.x) > 0 ? xd / std::abs(ray.x) : 1000.0;
		double yt = std::abs(ray.y) > 0 ? yd / std::abs(ray.y) : 1000.0;
		double zt = std::abs(ray.z) > 0 ? zd / std::abs(ray.z) : 1000.0;
		
		if (std::abs(ray.x) > 0 && xt > 0 && xt <= yt && xt <= zt)
		{
			bp_new.x += ray.x > 0.0 ? 1 : -1;
			s.x = xs;
			s.y = p.y + ray.y * (xs - p.x) / (ray.x);
			s.z = p.z + ray.z * (xs - p.x) / (ray.x);
		}
		else if (std::abs(ray.y) > 0 && yt > 0 && yt <= zt && yt <= zt)
		{
			bp_new.y += ray.y > 0.0 ? 1 : -1;
			s.x = p.x + ray.x * (ys - p.y) / (ray.y);
			s.y = ys;
			s.z = p.z + ray.z * (ys - p.y) / (ray.y);
		}
		else if (std::abs(ray.z) > 0)
		{
			bp_new.z += ray.z > 0.0 ? 1 : -1;
			s.x = p.x + ray.x * (zs - p.z) / (ray.z);
			s.y = p.y + ray.y * (zs - p.z) / (ray.z);
			s.z = zs;
		}
		else
		{
			return false;
		}
		
		if (math::length(s - p) > len)
			return false;
		
		CubeType cube = getCubeAt(bp_new);

		if (cube != 0)
		{
			result = bp_new;
			prev = bp;
			return true;
		}
	}
}

bool World::putBlock(const math::ivec3 &v, CubeType c, BlockData data)
{
	math::ivec3 chuckC = getChunkCoord(v);
	
	if (!isChunkCanBeRendered(eucDivChunk(v), true))
		return false;

	Chunk *chunk = getChunk(chuckC);
	math::ivec3 bp = eucModChunk(v);
	
	if (chunk->rawCubeAt(bp) == c)
		return false;
	
	chunk->put(bp, c);
	chunk->setBlockData(bp, data);
	
	math::ivec3 chunkPos = eucDivChunk(v) * CHUNK_SIZE_I;
	chunk->updateObjects(chunkPos, bp, c, data, modelTextures);

	std::vector<AddedBlockLight> addedBlocks, addedSunLightBlocks;
	std::vector<math::ivec3> removedBlocks, removedSunLightBlocks;
	
	if (c == 0)
	{
		removedBlocks.push_back(v);
		
		LightValue *pl = getLightRef<LIGHT_SUN>(IntCoord(v.x, v.y, v.z + 1));
		LightValue lv = pl ? *pl : MAX_LIGHT;
		LightValue sl = sunLightAt(v.z + 1);
		
		if (sl == lv)
		{
			for (int z = v.z; z >= 0; --z)
			{
				IntCoord p = IntCoord(v.x, v.y, z);
				CubeType cube = getCubeAt(p);
				if (cube > 0)
					break;
				
				pl = getLightRef<LIGHT_SUN>(p);
				if (!pl)
					break;
				
				AddedBlockLight lb;
				lb.position = p;
				lb.lightColor[LIGHT_SUN] = sunLightAt(z);
				addedSunLightBlocks.push_back(lb);
			}
		}
		else
		{
			removedSunLightBlocks.push_back(v);
		}
	}
	else
	{
		const Block *block = Block::get(c);
		
		AddedBlockLight lb;
		lb.position = v;
		lb.lightColor[LIGHT_R] = block ? block->lightValueR : 0;
		lb.lightColor[LIGHT_G] = block ? block->lightValueG : 0;
		lb.lightColor[LIGHT_B] = block ? block->lightValueB : 0;
		lb.lightColor[LIGHT_SUN] = 0;
		addedBlocks.push_back(lb);
		addedSunLightBlocks.push_back(lb);
		
		for (int z = v.z - 1; z >= 0; --z)
		{
			IntCoord p = IntCoord(v.x, v.y, z);
			CubeType cube = getCubeAt(p);

			if (cube > 0)
				break;

			LightValue *pl = getLightRef<LIGHT_SUN>(p);

			if (!pl)
				break;
			
			if (*pl < sunLightAt(z))
				break;

			removedSunLightBlocks.push_back(p);
		}
	}
	
	updateLight<LIGHT_R>(addedBlocks, removedBlocks);
	updateLight<LIGHT_G>(addedBlocks, removedBlocks);
	updateLight<LIGHT_B>(addedBlocks, removedBlocks);
	updateLight<LIGHT_SUN>(addedSunLightBlocks, removedSunLightBlocks);
	
	chunk->isDirty = true;
	
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

	worldShaderPass1.buildShaderProgram("wvs.glsl", "wgs.glsl", "wfs.glsl");
	worldShaderPass2.buildShaderProgram("wvs.glsl", "wgs2.glsl", "wfs2.glsl");
	shadowMapPass.buildShaderProgram("shadowvs.glsl", "shadowgs.glsl", "shadowfs.glsl");
	
	modelTextures.addTexture("textures/entity/wolf/wolf.png", "wolf");
	modelTextures.addTexture("textures/entity/chest/normal.png", "chest");
	modelTextures.upload();
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

bool World::isChunkCanBeRendered(const IntCoord &coord, bool ignoreBlockCount) const
{
	if (!isChunkCoordValid(coord))
		return false;
	
	ChunkMap::const_iterator it;
	
	it = chunks.find(coord);
	if (it == chunks.end() || !it->second->isLighted.load() || !it->second->isSunLighted.load())
		return false;
	
	if (!ignoreBlockCount && !it->second->opaqueBlockCount && !it->second->nonOpaqueBlockCount)
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
					if (isChunkCoordValid(c) && (it == chunks.end() || !it->second->isLighted.load() || !it->second->isSunLighted.load()))
						return false;
				}
			}
		}
	}
	
	return true;
}

int dirty1 = 0, dirty2 = 0;

Chunk *World::makeChunkLoadRequest(const IntCoord &coord)
{
	Chunk *chunk = new Chunk;
	pendingChunks[coord].reset(chunk);
	ChunkLoadRequest request;
	request.chunkCoord = coord;
	request.chunk = chunk;
	chunk->slpl = &sunLightPropagationLayerMap[coord];
	_pendingChunkCoords.push(request);
	
	return chunk;
}

extern int shadow_mode;

void World::renderShadowMap(const Camera &camera, const cyberCubes::Player &player, float scale)
{
	math::mat4 depthProjectionMatrix = math::ortho<float>(-scale,scale,-scale,scale,0,500);
	math::vec3 fw = camera.transform(1.0f, 0.0f, 0.0f);
	math::vec3 shift = scale * (fw - math::dot(fw, sunDir)*sunDir);
	//math::vec3 shift = scale * fw;
	math::vec3 shadowCenter = camera.position + shift;
	math::vec3 shadowEye = shadowCenter + sunDir*250.0f;
	math::mat4 depthViewMatrix = math::lookAt(shadowEye, shadowCenter, math::vec3(0,0,1));
	math::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;

	math::Frustum frustum;
	frustum.update(depthMVP);
	
	glUseProgram(shadowMapPass.program);
	
	if (shadowMapPass.uniforms.count("depthMVP"))
		glUniformMatrix4fv(shadowMapPass.uniforms["depthMVP"], 1, GL_FALSE, &depthMVP[0][0]);
	
	if (shadowMapPass.uniforms.count("mode"))
			glUniform1i(shadowMapPass.uniforms["mode"], shadow_mode);
	
	int renderCount = 0;
	for (IntCoord c : player.getChunks())
	{
		if (isChunkCanBeRendered(c, false))
		{
			math::vec3 bboxpos = i2f(c) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);
			math::BBox bbox(bboxpos, math::vec3(CHUNK_SIZE_F, CHUNK_SIZE_F, CHUNK_SIZE_F));

			if (frustum.checkBBox(bbox) != math::IntersectionType::OUTSIDE)
			{
				math::vec3 pos = i2f(c) * CHUNK_SIZE_F;
				glUniform3fv(shadowMapPass.uniforms["chunkPosition"], 1, &pos[0]);

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
					
				if (chunk.opaqueBlockCount)
				{
					rd.renderShadowMap(_renderTick, -sunDir);
					++renderCount;
				}
			}
		}
	}
	
	//std::cout << "SHADOW RENDERED " << renderCount << " | " << scale << std::endl;
}

void World::renderPass1(const Camera &camera, const cyberCubes::Player &player, std::vector<IntCoord> &nonOpaqueChunks, GLuint blockTexture, GLuint detailTexture,
			GLuint shadow_map_texture1, GLuint shadow_map_texture2, GLuint box_positions_texture1, GLuint box_positions_texture2, float shadow_scale1, float shadow_scale2,
			GLuint HSColorTexture
       		)
{
	++_renderTick;
	
	glDisable(GL_BLEND);
	
	math::mat4 MVP = camera.getVP();
	math::mat4 MV = camera.getMatrix();
	math::Frustum frustum;
	frustum.update(MVP);
	
	
	/////////////////
	
	math::mat4 depthProjectionMatrix1 = math::ortho<float>(-shadow_scale1,shadow_scale1,-shadow_scale1,shadow_scale1, 0,500);
	math::mat4 depthProjectionMatrix2 = math::ortho<float>(-shadow_scale2,shadow_scale2,-shadow_scale2,shadow_scale2, 0,500);
	
	math::vec3 fw = camera.transform(1.0f, 0.0f, 0.0f);
	
	math::vec3 shift1 = shadow_scale1 * (fw - math::dot(fw, sunDir)*sunDir);
	//math::vec3 shift1 = shadow_scale1 * fw;
	math::vec3 shadowCenter1 = camera.position + shift1;
	math::vec3 shadowEye1 = shadowCenter1 + sunDir*250.0f;
	math::mat4 depthViewMatrix1 = math::lookAt(shadowEye1, shadowCenter1, math::vec3(0,0,1));
	
	math::vec3 shift2 = shadow_scale2 * (fw - math::dot(fw, sunDir)*sunDir);
	//math::vec3 shift2 = shadow_scale2 * fw;
	math::vec3 shadowCenter2 = camera.position + shift2;
	math::vec3 shadowEye2 = shadowCenter2 + sunDir*250.0f;
	math::mat4 depthViewMatrix2 = math::lookAt(shadowEye2, shadowCenter2, math::vec3(0,0,1));
	
	math::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
	math::mat4 depthBiasMVP1 = biasMatrix * depthProjectionMatrix1 * depthViewMatrix1;
	math::mat4 depthBiasMVP2 = biasMatrix * depthProjectionMatrix2 * depthViewMatrix2;
	
	/////////////////
	
	cyberCubes::GeomOcclusitionCulling culling(camera.position);
	int oq_tested = 0;
	int culled1 = 0;
	int culled2 = 0;
	int oq_checks = 0;

	typedef std::multimap<float, IntCoord> VisibleChunksMap;
	VisibleChunksMap visibleChunks;
	
	std::multimap<float, IntCoord> playerChunks;
	for (IntCoord coord : player.getChunks())
	{
		math::vec3 d = i2f(coord)*CHUNK_SIZE_F + math::vec3(CHUNK_SIZE_F / 2.0f - 0.5f, CHUNK_SIZE_F / 2.0f - 0.5f, CHUNK_SIZE_F / 2.0f - 0.5f) - player.getPosition();
		float dist = math::length(d);
		playerChunks.insert(std::make_pair(dist, coord));
	}
	
	for (std::multimap<float, IntCoord>::iterator it = playerChunks.begin(); it != playerChunks.end(); ++it)
	{
		IntCoord coord = it->second;
		
		Chunk *chunk;

		if (chunks.count(coord))
		{
			chunk = chunks[coord].get();
		}
		else
		{
			if (!pendingChunks.count(coord))
			{
				chunk = makeChunkLoadRequest(coord);
			}
			else
			{
				chunk = pendingChunks[coord].get();
			}
		}
		
		chunk->touchTick = _renderTick;
		
		bool slplLoaded = true;
		
		if (coord.z >= 0)
		{
			for (int z = coord.z + 1; z < SZ; ++z)
			{
				IntCoord p = IntCoord(coord.x, coord.y, z);
				if (sunLightPropagationLayerMap.count(p))
				{
					SunLightPropagationLayer &l = sunLightPropagationLayerMap[p];
					if (l.isCannotBeLoaded.load() && !chunks.count(p) && !pendingChunks.count(p))
					{
						Chunk *ch = makeChunkLoadRequest(p);
					}
				}
				else
				{
					SunLightPropagationLayer &l = sunLightPropagationLayerMap[p];
					SLLayerLoadRequest request;
					request.chunkCoord = p;
					request.layer = &l;
					_slLayerCoords.push(request);
				}
			}
			
			
			for (int z = coord.z + 1; z < SZ; ++z)
			{
				IntCoord p = IntCoord(coord.x, coord.y, z);
				SunLightPropagationLayer &l = sunLightPropagationLayerMap[p];
				if (!l.isLoaded.load())
				{
					slplLoaded = false;
					break;
				}
			}
		}
		
		if (chunks.count(coord) && !chunk->isSunLighted.load() && slplLoaded && !chunk->sunLightRecalculating)
		{
			chunk->sunLightRecalculating = true;
			
			if (coord.z >= 0)
			{
				// TODO code dublication
				ChunkLightUpRequest request;
				request.chunkCoord = coord;
				request.isSunLight = true;
				request.chunk = chunk;
				SunLightPropagationSum *sum = new SunLightPropagationSum;
				request.sunLightPropagationSum.reset(sum);
				_getCurrentChunks3x3x3Area(request.chunkPtrMap, coord);
				
				for (int z = coord.z + 1; z < SZ; ++z)
					sum->add(sunLightPropagationLayerMap[IntCoord(coord.x, coord.y, z)]);
				
				_lightingChunkCoords.push(std::move(request));
			}
			else
			{
				chunk->isSunLighted.store(true);
			}
		}
		
		if (isChunkCanBeRendered(coord, false))
		{
			math::vec3 pos = i2f(coord) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);
			math::BBox bbox(pos, math::vec3(CHUNK_SIZE_F, CHUNK_SIZE_F, CHUNK_SIZE_F));

			if (frustum.checkBBox(bbox) != math::IntersectionType::OUTSIDE)
			{
				pos += math::vec3(CHUNK_SIZE_F / 2.0f, CHUNK_SIZE_F / 2.0f, CHUNK_SIZE_F / 2.0f) - camera.position;
				float dist2 = math::dot(pos, pos);
				visibleChunks.insert(std::make_pair(dist2, coord));
			}
		}
	}
	
	for (ChunkMap::iterator it = pendingChunks.begin(); it != pendingChunks.end();)
	{
		Chunk *chunk = it->second.get();

		if (chunk->isLoaded.load())
		{
			IntCoord coord = it->first;
			chunks.emplace(std::make_pair(coord, std::move(it->second)));
			_linkChunk(chunk, coord);
			
			// TODO code dublication
			ChunkLightUpRequest request;
			request.chunkCoord = coord;
			request.isSunLight = false;
			request.chunk = chunk;
			_getCurrentChunks3x3x3Area(request.chunkPtrMap, coord);
			_lightingChunkCoords.push(std::move(request));
			
			it = pendingChunks.erase(it);
			continue;
		}
		
		++it;
	}

	std::vector<IntCoord> visibleChunksVector;
	std::vector<bool> oqVisibleChunksVector;

	for (VisibleChunksMap::iterator it = visibleChunks.begin(); it != visibleChunks.end(); ++it)
	{
		visibleChunksVector.push_back(it->second);
	}

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
		glUseProgram(worldShaderPass1.program);

		if (worldShaderPass1.uniforms.count("MVP"))
			glUniformMatrix4fv(worldShaderPass1.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
		
		if (worldShaderPass1.uniforms.count("MV"))
			glUniformMatrix4fv(worldShaderPass1.uniforms["MV"], 1, GL_FALSE, &MV[0][0]);
		
		if (worldShaderPass1.uniforms.count("fogFar"))
			glUniform1f(worldShaderPass1.uniforms["fogFar"], player.getFogFar());
		
		if (worldShaderPass1.uniforms.count("blockSampler"))
		{
			glUniform1i(worldShaderPass1.uniforms["blockSampler"], 0);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
		}
		
		if (worldShaderPass1.uniforms.count("detailSampler"))
		{
			glUniform1i(worldShaderPass1.uniforms["detailSampler"], 1);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, detailTexture);
		}
		
		if (worldShaderPass1.uniforms.count("shadowSampler1"))
		{
			glUniform1i(worldShaderPass1.uniforms["shadowSampler1"], 2);
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, shadow_map_texture1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		}
		
		if (worldShaderPass1.uniforms.count("shadowSampler2"))
		{
			glUniform1i(worldShaderPass1.uniforms["shadowSampler2"], 3);
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, shadow_map_texture2);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		}
		
		if (worldShaderPass1.uniforms.count("shadowBoxPosSampler1"))
		{
			glUniform1i(worldShaderPass1.uniforms["shadowBoxPosSampler1"], 4);
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, box_positions_texture1);
		}
		
		if (worldShaderPass1.uniforms.count("shadowBoxPosSampler2"))
		{
			glUniform1i(worldShaderPass1.uniforms["shadowBoxPosSampler2"], 5);
			glActiveTexture(GL_TEXTURE0 + 5);
			glBindTexture(GL_TEXTURE_2D, box_positions_texture2);
		}
		
		if (worldShaderPass2.uniforms.count("HSColorSampler"))
		{
			glUniform1i(worldShaderPass2.uniforms["HSColorSampler"], 6);
			glActiveTexture(GL_TEXTURE0 + 6);
			glBindTexture(GL_TEXTURE_2D, HSColorTexture);
		}
		
		
		if (worldShaderPass1.uniforms.count("depthBiasMVP1"))
			glUniformMatrix4fv(worldShaderPass1.uniforms["depthBiasMVP1"], 1, GL_FALSE, &depthBiasMVP1[0][0]);
		if (worldShaderPass1.uniforms.count("depthBiasMVP2"))
			glUniformMatrix4fv(worldShaderPass1.uniforms["depthBiasMVP2"], 1, GL_FALSE, &depthBiasMVP2[0][0]);
		if (worldShaderPass1.uniforms.count("sunLightDir"))
		{
			math::vec3 sunLightDir = math::normalize(sunDir);
			
			glUniform3fv(worldShaderPass1.uniforms["sunLightDir"], 1, &sunLightDir[0]);
		}
		if (worldShaderPass1.uniforms.count("sunLightDirInv"))
		{
			math::vec3 sunLightDirInv = 1.0f/math::normalize(sunDir);
			
			glUniform3fv(worldShaderPass1.uniforms["sunLightDirInv"], 1, &sunLightDirInv[0]);
		}
		
		if (worldShaderPass1.uniforms.count("sunLightDir"))
		{
			math::vec3 sunLightDir = math::normalize(sunDir);
			
			glUniform3fv(worldShaderPass1.uniforms["sunLightDir"], 1, &sunLightDir[0]);
		}
		
		if (worldShaderPass1.uniforms.count("cameraPos"))
		{
			glUniform3fv(worldShaderPass1.uniforms["cameraPos"], 1, &camera.position[0]);
		}
		
		if (worldShaderPass1.uniforms.count("mode"))
			glUniform1i(worldShaderPass1.uniforms["mode"], shadow_mode);

		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			if (oqVisibleChunksVector[i])
			{
				IntCoord &c = visibleChunksVector[i];
				math::vec3 pos = i2f(c) * CHUNK_SIZE_F;
				glUniform3fv(worldShaderPass1.uniforms["chunkPosition"], 1, &pos[0]);
				glUniform1f(worldShaderPass1.uniforms["dayNightLightCoef"], dayNightLightCoef);

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
				
				if (chunk.opaqueBlockCount)
				{
					rd.render(-1, camera.position - pos, _renderTick, RenderKind::OPAQUE, math::vec3());
					trisRendred += rd.trisRendered;
				}
				
				if (chunk.nonOpaqueBlockCount)
					nonOpaqueChunks.push_back(c);
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

			math::vec3 pos = i2f(c) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);
			
			oq_tested++;
			bool geom_occlusition = culling.isOccluded(pos);
			//bool geom_occlusition = false;
			if (geom_occlusition)
			{
				oqVisibleChunksVector.push_back(false);
				culled1++;
			}
			else
			{
				oqVisibleChunksVector.push_back(true);
				GLuint queryId = oqIds[c] = queryPool.getQueryId();
				glBeginQuery(GL_ANY_SAMPLES_PASSED, queryId);
				//std::cout << "Query " << c.get<0>() << " " << c.get<1>() << " " << c.get<2>() << " " << oqIds[c] << std::endl;
				glUniform3fv(oqShader.uniforms["chunkPosition"], 1, &pos[0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

				glEndQuery(GL_ANY_SAMPLES_PASSED);
			}
		}

		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			if (oqVisibleChunksVector[i])
			{
				IntCoord &c = visibleChunksVector[i];
				GLuint params;
				GLuint queryId = oqIds[c];
				glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, &params);
				queryPool.releaseQueryId(queryId);
				oqVisibleChunksVector[i] = (params == 1);
				
				oq_checks++;
				if (params == 0)
				{
					math::vec3 pos = i2f(c) * CHUNK_SIZE_F - math::vec3(0.5f, 0.5f, 0.5f);
					culling.addOccluder(pos);
					culled2++;
				}
			}
		}

		//glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	
	/////////////////////////////////////////////////////////////////////////
	math::vec2 invTextureSize = modelRenderer.getInvTextureSize();
	std::vector<cyberCubes::model::Vertex> modelBuffer;
	for (const IntCoord &c : nonOpaqueChunks)
	{
		Chunk &chunk = *chunks[c];
		for (auto &p : chunk.objects)
		{
			math::ivec3 pos = ind2c(p.first);
			cyberCubes::model::Model *model = p.second.get();
			glm::ivec4 light = chunk.rawLightAt<LIGHT_SUNRGBA>(pos);
			
			model->colors[0] = (unsigned char) (light.x*255.0/MAX_LIGHT_F);
			model->colors[1] = (unsigned char) (light.y*255.0/MAX_LIGHT_F);
			model->colors[2] = (unsigned char) (light.z*255.0/MAX_LIGHT_F);
			model->colors[3] = (unsigned char) (light.w*255.0/MAX_LIGHT_F);
			
			model->render(modelBuffer, invTextureSize);
		}
		
		
	}
	
	if (!modelBuffer.empty())
	{
		modelRendererBuffer.upload(modelBuffer);
		
		modelRenderer.startRender();
		modelRenderer.render(modelRendererBuffer, MVP);
		modelRenderer.finishRender();
	}
	
	
	/////////////////////////////////////////////////////////////////////////

	extern int g_trisRendered ;
	g_trisRendered = trisRendred;
	//std::cout << "triangles " << trisRendred << " OQ " << occluded  << "/" << visibleChunksVector.size() << " memUse: " << _getTotalRenderDataMemoryUse()/1024.0/1024.0 << " MB" << std::endl;
	//std::cout << "Chunk Mem " << chunks.size()*sizeof(Chunk)/1024.0/1024.0 << " MB, " << pendingChunks.size()*sizeof(Chunk)/1024.0/1024.0 << " MB" << std::endl;
	
	//std::cout << dirty1 << " " << dirty2 << std::endl;
	
	//std::cout << "CULLED " << culled1 << " " << culled2 << " " << oq_checks << " " << oq_tested << " " << occluded << std::endl;
	
	releaseOldRenderData();
	_unloadUnusedChunks();
}

void World::renderPass2(const Camera &camera, const cyberCubes::Player &player, const std::vector<IntCoord> &nonOpaqueChunks, GLuint blockTexture, GLuint HSColorTexture)
{
	if (!nonOpaqueChunks.size())
		return;
	
	math::mat4 MVP = camera.getVP();
	math::mat4 MV = camera.getMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	//glBlendFunc(GL_ONE, GL_ONE);
	
	glEnable(GL_CLIP_DISTANCE0 + 0);
	glEnable(GL_CLIP_DISTANCE0 + 1);
	glEnable(GL_CLIP_DISTANCE0 + 2);
	
	glUseProgram(worldShaderPass2.program);

	if (worldShaderPass2.uniforms.count("MVP"))
		glUniformMatrix4fv(worldShaderPass2.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
		
	if (worldShaderPass2.uniforms.count("MV"))
		glUniformMatrix4fv(worldShaderPass2.uniforms["MV"], 1, GL_FALSE, &MV[0][0]);
	
	if (worldShaderPass2.uniforms.count("fogFar"))
		glUniform1f(worldShaderPass2.uniforms["fogFar"], player.getFogFar());
	
	if (worldShaderPass2.uniforms.count("lightMultiplier"))
		glUniform1f(worldShaderPass2.uniforms["lightMultiplier"], lightMultiplier);

	if (worldShaderPass2.uniforms.count("fogColor"))
		glUniform3fv(worldShaderPass2.uniforms["fogColor"], 1, &fogColor[0]);
	
	if (worldShaderPass2.uniforms.count("eyePos"))
		glUniform3fv(worldShaderPass2.uniforms["eyePos"], 1, &camera.position[0]);

			
	if (worldShaderPass2.uniforms.count("blockSampler"))
	{
		glUniform1i(worldShaderPass2.uniforms["blockSampler"], 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
	}
	
	if (worldShaderPass2.uniforms.count("HSColorSampler"))
	{
		glUniform1i(worldShaderPass2.uniforms["HSColorSampler"], 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, HSColorTexture);
	}
	
	GLint clipDirLocation = -1;
	if (worldShaderPass2.uniforms.count("clipDir"))
		clipDirLocation = worldShaderPass2.uniforms["clipDir"];
	
	std::multimap<float, IntCoord> playerChunks;
	for (const IntCoord &coord : nonOpaqueChunks)
	{
		math::vec3 d = i2f(coord)*CHUNK_SIZE_F + math::vec3(CHUNK_SIZE_F / 2.0f - 0.5f, CHUNK_SIZE_F / 2.0f - 0.5f, CHUNK_SIZE_F / 2.0f - 0.5f) - player.getPosition();
		float dist = math::length(d);
		playerChunks.insert(std::make_pair(-dist, coord));
	}
	
	for (std::multimap<float, IntCoord>::iterator it = playerChunks.begin(); it != playerChunks.end(); ++it)
	{
		IntCoord c = it->second;
		
		math::vec3 pos = i2f(c) * CHUNK_SIZE_F;
		if (worldShaderPass2.uniforms.count("chunkPosition"))
			glUniform3fv(worldShaderPass2.uniforms["chunkPosition"], 1, &pos[0]);
		
		if (worldShaderPass2.uniforms.count("dayNightLightCoef"))
			glUniform1f(worldShaderPass2.uniforms["dayNightLightCoef"], dayNightLightCoef);
		
		RenderData &rd = *renderData[c];
		rd.render(clipDirLocation, camera.position - pos, _renderTick, RenderKind::NON_OPAQUE, math::vec3());
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CLIP_DISTANCE0 + 0);
	glDisable(GL_CLIP_DISTANCE0 + 1);
	glDisable(GL_CLIP_DISTANCE0 + 2);
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

void World::_getCurrentChunks3x3x3Area(ChunkPtrMap &m, const IntCoord &center)
{
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				IntCoord p = center + IntCoord(x, y, z);
				if (chunks.count(p))
				{
					m[p] = chunks[p].get();
				}
				else
				{
					m[p] = solidChunk.get();
				}
			}	
		}	
	}
}
