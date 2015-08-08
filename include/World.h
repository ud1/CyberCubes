#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <unordered_map>
#include <memory>
#include <GL/glew.h>
#include <vector>
#include "Math.h"
#include "Frustum.h"
#include "Shader.h"
#include "Camera.h"
#include "WorldProvider.h"
#include "Chunk.h"
#include "Player.h"
#include "BlockingQueue.hpp"
#include "GLQueryPool.h"

#include <thread>
#include <memory>
#include <atomic>

struct RenderData;

struct AddedBlockLight
{
	math::ivec3 position;
	LightValue lightColor[LIGHT_COUNT];

	template<LightType lt>
	LightValue getLight() const
	{
		return lightColor[lt];
	}
};

class World
{
public:
	World();
	~World();
	
	typedef std::unique_ptr<Chunk> PChunk;
	typedef std::unique_ptr<RenderData> PRenderData;
	typedef std::unique_ptr<SunLightPropagationSum> PSunLightPropagationSum;

	PChunk solidChunk;

	typedef std::unordered_map<IntCoord, Chunk *, IntCoordHasher> ChunkPtrMap;
	typedef std::unordered_map<IntCoord, PChunk, IntCoordHasher> ChunkMap;
	ChunkMap chunks, pendingChunks;
	
	typedef std::unordered_map<IntCoord, PRenderData, IntCoordHasher> RenderDataMap;
	RenderDataMap renderData;
	
	typedef std::unordered_map<IntCoord, SunLightPropagationLayer, IntCoordHasher> SunLightPropagationLayerMap;
	SunLightPropagationLayerMap sunLightPropagationLayerMap;
	
	cyberCubes::GLQueryPool queryPool;

	WorldProvider worldProvider;

	GLuint oqVao = 0;
	GLuint oqTriangleBufferObject = 0;
	GLuint oqTriangleIndicesBufferObject = 0;
	Shader oqShader;
	Shader worldShader;
	float dayNightLightCoef;

	void create();
	void render(const Camera &camera, const cyberCubes::Player &player);
	float getMaxLightNearPoint(const math::vec3 &v);

	template<LightType lt>
	float getMaxLightAtPoint(const math::vec3 &v);

	CubeType getCubeAt(const math::ivec3 &v);

	template<LightType lt>
	LightValue *getLightRef(const math::ivec3 &v);

	template<LightType lt>
	void updateLight(const std::vector<AddedBlockLight> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
	bool getBlock(const math::vec3 &p, const math::vec3 &ray, float len, math::ivec3 &result, math::ivec3 &prev);
	void move(math::BBox &box, const math::vec3 &delta);
	bool putBlock(const math::ivec3 &v, CubeType c);

	bool isChunkCoordValid(const IntCoord &p) const;
	
	
private:
	IntCoord minWorldChunkCoord, maxWorldChunkCoord;
	
	void _loadChunks();
	void _lightChunks();
	void _unloadUnusedChunks();
	void _loadSLLayers();
	
	struct ChunkLoadRequest
	{
		IntCoord chunkCoord;
		Chunk *chunk;
	};
	
	struct ChunkLightUpRequest
	{
		IntCoord chunkCoord;
		bool isSunLight;
		Chunk *chunk;
		PSunLightPropagationSum sunLightPropagationSum;
		ChunkPtrMap chunkPtrMap;
	};
	
	struct SLLayerLoadRequest
	{
		IntCoord chunkCoord;
		SunLightPropagationLayer *layer;
	};
	
	cyberCubes::BlockingQueue<ChunkLoadRequest> _pendingChunkCoords;
	std::unique_ptr<std::thread> _loadChunksThread;
	std::atomic_flag _isLoadingThreadRunned;
	
	cyberCubes::BlockingQueue<ChunkLightUpRequest> _lightingChunkCoords;
	std::unique_ptr<std::thread> _lightingChunksThread;
	std::atomic_flag _isLightingThreadRunned;
	
	cyberCubes::BlockingQueue<SLLayerLoadRequest> _slLayerCoords;
	std::unique_ptr<std::thread> _loadSLLayerThread;
	std::atomic_flag _isLoadSLLayerThreadRunned;
	
	Chunk *getChunk(const IntCoord &coord);
	
	void _linkChunk(Chunk *chunk, const IntCoord &pos);
	void _unlinkChunk(const IntCoord &pos);
	size_t _getTotalRenderDataMemoryUse() const;
	void releaseOldRenderData();
	bool isChunkCanBeRendered(const IntCoord &coord, bool ignoreBlockCount) const;
	
	Tick _renderTick;
	size_t _maxRenderDataMemoryUse;
	size_t _maxChunksMemoryUse;
	
	Chunk *makeChunkLoadRequest(const IntCoord &coord);
	void _getCurrentChunks3x3x3Area(ChunkPtrMap &m, const IntCoord &center);
	
	struct Accessor
	{
		Accessor(World *world) : world(world) {}
		
		World *world;
		
		template<LightType lt>
		LightValue *getLightRef(const math::ivec3 &v)
		{
			return world->getLightRef<lt>(v);
		}
		
		CubeType getCubeAt(const math::ivec3 &v)
		{
			return world->getCubeAt(v);
		}
		
		Chunk *getChunk(const IntCoord &coord)
		{
			return world->getChunk(coord);
		}
	};
};

#endif // WORLD_H
