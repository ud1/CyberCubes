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

const int S = 32;
const int SZ = 4;
long cubeCount = 0;

class GradientModule : public noise::module::Module
{
public:

	GradientModule() : Module(0) {}
	virtual int GetSourceModuleCount () const
	{
		return 0;
	}

	virtual double GetValue (double x, double y, double z) const
	{
		return (z) / 50.0;
	}
};

float World::getMaxLightNearPoint(const math::vec3 &v)
{
    float res = getMaxLightAtPoint(v);
    for (float i = -2.0; i <= 2.0; ++i)
    {
		for (float j = -2.0; j <= 2.0; ++j)
		{
			for (float k = -2.0; k <= 2.0; ++k)
			{
				res = std::max(res, getMaxLightAtPoint(v + math::vec3(i, j, k)));
			}
		}
    }
    return res;
}

float World::getMaxLightAtPoint(const math::vec3 &v)
{
	math::ivec3 p = math::ivec3(v.x, v.y, v.z);
    if (v.x < 0 || v.x >= S*CHUNK_SIZE)
		return 1.0;
	if (v.y < 0 || v.y >= S*CHUNK_SIZE)
		return 1.0;
	if (v.z < 0 || v.z >= SZ*CHUNK_SIZE)
		return 1.0;

	LightValue l = chunks[boost::make_tuple(p.x / CHUNK_SIZE, p.y / CHUNK_SIZE, p.z / CHUNK_SIZE)]->lightAt(math::ivec3(p.x % CHUNK_SIZE, p.y % CHUNK_SIZE, p.z % CHUNK_SIZE));
	return (float) l / (float) MAX_LIGHT;
}

CubeType World::getCubeAt(const math::ivec3 &v)
{
	if (v.x < 0 || v.x >= S*CHUNK_SIZE)
		return 0;
	if (v.y < 0 || v.y >= S*CHUNK_SIZE)
		return 0;
	if (v.z < 0 || v.z >= SZ*CHUNK_SIZE)
		return 0;

	return chunks[boost::make_tuple(v.x / CHUNK_SIZE, v.y / CHUNK_SIZE, v.z / CHUNK_SIZE)]->cubeAt(math::ivec3(v.x % CHUNK_SIZE, v.y % CHUNK_SIZE, v.z % CHUNK_SIZE));
}

LightValue *World::getLightRef(const math::ivec3 &v)
{
	if (v.x < 0 || v.x >= S*CHUNK_SIZE)
		return nullptr;
	if (v.y < 0 || v.y >= S*CHUNK_SIZE)
		return nullptr;
	if (v.z < 0 || v.z >= SZ*CHUNK_SIZE)
		return nullptr;

	return &chunks[boost::make_tuple(v.x / CHUNK_SIZE, v.y / CHUNK_SIZE, v.z / CHUNK_SIZE)]->lightRefAt(math::ivec3(v.x % CHUNK_SIZE, v.y % CHUNK_SIZE, v.z % CHUNK_SIZE));
}

struct CoordHasher
{
	std::size_t operator()(const math::ivec3 &k) const
	{
		return ((k.x * 256) + k.y) * 256 + k.z;
	}
};

math::ivec3 getChunkCoord(const math::ivec3 &p)
{
	math::ivec3 res = p;
	if (res.x < 0)
		res.x -= (CHUNK_SIZE - 1);
	if (res.y < 0)
		res.y -= (CHUNK_SIZE - 1);
	if (res.z < 0)
		res.z -= (CHUNK_SIZE - 1);

	return res / math::ivec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
}

void adj(int i, math::ivec3 &p)
{
	switch (i)
	{
		case 0: p.x--; break;
		case 1: p.x++; break;
		case 2: p.y--; break;
		case 3: p.y++; break;
		case 4: p.z--; break;
		case 5: p.z++; break;
	}
}

struct UpdateLightSets
{
	std::vector<math::ivec3> blockSet, blockSet2;

	UpdateLightSets()
	{
		blockSet.reserve(256*256*256);
		blockSet2.reserve(256*256*256);
	}

	void clear()
	{
		blockSet.clear();
		blockSet2.clear();
	}

} updateLightSets;

void World::updateLight(const std::vector<math::ivec3> &addedBlocks, const std::vector<math::ivec3> &removedBlocks)
{
	int cnt = 0;
	int cnt2 = 0;

	std::unordered_set<math::ivec3, CoordHasher> lightSources;
	std::unordered_set<math::ivec3, CoordHasher> updatedChunks;
	updateLightSets.clear();

	for (const math::ivec3 &blockCoord : addedBlocks)
	{
		boost::timer::auto_cpu_timer t;

		LightValue *lp = getLightRef(blockCoord);
		if (!lp)
			continue;

		std::vector<math::ivec3> &blockSet = updateLightSets.blockSet, &blockSet2 = updateLightSets.blockSet2;

		LightValue l = *lp;
		*lp = 0;
		blockSet.push_back(blockCoord);

		while (l-- > 1 && !blockSet.empty())
		{
			for (const math::ivec3 &p : blockSet)
			{
				for (int i = 0; i < 6; ++i)
				{
					math::ivec3 adjP = p;
					adj(i, adjP);
					LightValue *l2 = getLightRef(adjP);
					if (l2 && *l2 && !getCubeAt(adjP))
					{
						if (*l2 <= l)
						{
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

	std::cout << "ZeroB " << cnt << " " << cnt2 << std::endl;

	for (const math::ivec3 &p : removedBlocks)
	{
		for (int i = 0; i < 6; ++i)
		{
			math::ivec3 adjP = p;
			adj(i, adjP);

			LightValue *l2 = getLightRef(adjP);
			if (l2 && *l2 > 1)
			{
				lightSources.insert(adjP);
			}
		}
	}

	std::cout << "lightSources " << lightSources.size() << std::endl;
	cnt = 0;
	boost::timer::auto_cpu_timer t;

	std::unordered_set<math::ivec3, CoordHasher> lightSources2;
	while (!lightSources.empty())
	{
		for (const math::ivec3 &p : lightSources)
		{
			LightValue l1 = *getLightRef(p);

			if (l1-- > 1)
			{
				for (int i = 0; i < 6; ++i)
				{
					math::ivec3 adjP = p;
					adj(i, adjP);

					LightValue *l2 = getLightRef(adjP);
					if (l2 && *l2 < l1 && !getCubeAt(adjP))
					{
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

	for (const math::ivec3 &p : updatedChunks)
	{
		if (p.x < 0 || p.x >= S)
			continue;
		if (p.y < 0 || p.y >= S)
			continue;
		if (p.z < 0 || p.z >= SZ)
			continue;
		chunks[boost::make_tuple(p.x, p.y, p.z)]->isDirty = true;
	}
}

bool World::getBlock(const math::vec3 &p, const math::vec3 &ray, float len, math::ivec3 &result, math::ivec3 &prev)
{
	const float STEP = 0.01f;

	math::vec3 s = p + math::vec3(0.5f, 0.5f, 0.5f);
	math::vec3 d = ray * STEP;


	math::ivec3 bp = (math::ivec3) p;

    for (;(len -= STEP) > 0.0f;)
    {
        s += d;
        math::ivec3 bp_new = (math::ivec3) s;
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
	if (chuckC.x < 0 || chuckC.x >= S)
		return false;
	if (chuckC.y < 0 || chuckC.y >= S)
		return false;
	if (chuckC.z < 0 || chuckC.z >= SZ)
		return false;

	math::ivec3 bp = v % math::ivec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
	chunks[boost::make_tuple(chuckC.x, chuckC.y, chuckC.z)]->cubes[((bp.x * CHUNK_SIZE) + bp.y) * CHUNK_SIZE + bp.z] = c;
	chunks[boost::make_tuple(chuckC.x, chuckC.y, chuckC.z)]->isDirty = true;

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

	int iterN = (int) (len * 2.0f + 1.0);
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

    std::cout << "create data" << std::endl;
    for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                chunks[boost::make_tuple(i, j, k)].reset(new Chunk);
                renderData[boost::make_tuple(i, j, k)].reset(new RenderData);
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
                Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];

                chunk.l = (i == 0)       ? pSolidChunk : chunks[boost::make_tuple(i - 1, j, k)].get();
                chunk.r = (i == (S - 1)) ? pSolidChunk : chunks[boost::make_tuple(i + 1, j, k)].get();

                chunk.b = (j == 0)       ? pSolidChunk : chunks[boost::make_tuple(i, j - 1, k)].get();
                chunk.f = (j == (S - 1)) ? pSolidChunk : chunks[boost::make_tuple(i, j + 1, k)].get();

                chunk.d = (k == 0)       ? pSolidChunk : chunks[boost::make_tuple(i, j, k - 1)].get();
                chunk.u = (k == (SZ - 1)) ? pSolidChunk : chunks[boost::make_tuple(i, j, k + 1)].get();

                std::cout << i << " " << j << " " << k << " " << chunk.d->isDummy << std::endl;
            }
        }
    }

	GradientModule gradientModule;
    noise::module::Turbulence turbulence;
    turbulence.SetSourceModule(0, gradientModule);
    turbulence.SetFrequency (0.03);
	turbulence.SetPower (40.0);

    std::cout << "Fill" << std::endl;
    for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];

                if (worldProvider.load(chunk, math::ivec3(i, j, k)))
					continue;

				for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
				{
					for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
					{
						for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
						{
							math::ivec3 p(x, y, z);
							math::vec3 pos = math::vec3(i * CHUNK_SIZE + p.x, j * CHUNK_SIZE + p.y, k * CHUNK_SIZE + p.z);

							double v = turbulence.GetValue(pos.x, pos.y, pos.z);

							if (v < 1.0 || v > 1.8)
							{
								chunk.put(p, ((float) rand() / (float) RAND_MAX > (pos.z / 90.0)) ? 1 : 2);
								cubeCount++;
							}
						}
					}
				}
            }
        }
    }

    /*std::cout << "updateSunLight" << std::endl;
	for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];
                chunk.updateSunLight();
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
                    Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];
                    updCount += chunk.updateLightIter();
                }
            }
        }

        if (!updCount)
			break;
    }*/

	std::cout << "saveData" << std::endl;
    for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];
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
                Chunk &chunk = *chunks[boost::make_tuple(i, j, k)];
                RenderData &rd = *renderData[boost::make_tuple(i, j, k)];
                rd.addVertexData(chunk);
                rd.uploadData();
                videoMemUse += rd.getVideoMemUse();
            }
        }
    }
    std::cout << "videoMemUse " << videoMemUse << std::endl;

    GLuint oqIdsAr[S*S*S];
    glGenQueries(S*S*S, oqIdsAr);
    for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                oqIds[boost::make_tuple(i, j, k)] = oqIdsAr[(i*S + j)*S + k];
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
    float oqVertices[] = {
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
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

    if (oqTriangleIndicesBufferObject == 0)
        glGenBuffers(1, &oqTriangleIndicesBufferObject); //create the buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oqTriangleIndicesBufferObject); //we're "using" this one now

    int oqIndeces[] = {
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


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind any buffers

    printf("OQ glError: %d\n", glGetError());

    oqShader.buildShaderProgram("oqvs.glsl", "oqfs.glsl");
    ////////////////////////////////////////

    worldShader.buildShaderProgram("wvs.glsl", "wgs.glsl",
    "wfs.glsl");
}

void World::render(const Camera &camera)
{
	math::mat4 MVP = camera.getVP();
	math::Frustum frustum;
    frustum.update(MVP);

	typedef std::multimap<float, boost::tuple<int, int, int> > VisibleChunksMap;
	VisibleChunksMap visibleChunks;
	for (int i = 0; i < S; ++i)
    {
        for (int j = 0; j < S; ++j)
        {
            for (int k = 0; k < SZ; ++k)
            {
                math::vec3 pos = math::vec3(i*CHUNK_SIZE, j*CHUNK_SIZE, k*CHUNK_SIZE) - math::vec3(0.5f, 0.5f, 0.5f);
                math::BBox bbox(pos, math::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));

                if (frustum.checkBBox(bbox) != math::IntersectionType::OUTSIDE)
                {
                    pos += math::vec3(CHUNK_SIZE/2, CHUNK_SIZE/2, CHUNK_SIZE/2) - camera.position;
                    float dist2 = math::dot(pos, pos);
					visibleChunks.insert(std::make_pair(dist2, boost::make_tuple(i, j, k)));
                }
            }
        }
    }

    std::vector<boost::tuple<int, int, int> > visibleChunksVector;
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

	for (size_t n = 0; n < visibleChunksVector.size(); )
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
				boost::tuple<int, int, int> &c = visibleChunksVector[i];
				math::vec3 pos = math::vec3(c.get<0>()*CHUNK_SIZE, c.get<1>()*CHUNK_SIZE, c.get<2>()*CHUNK_SIZE);
				glUniform3fv(worldShader.uniforms["chunkPosition"], 1, &pos[0]);

				RenderData &rd = *renderData[c];

				Chunk &chunk = *chunks[c];
				if (chunk.isDirty)
				{
					chunk.isDirty = false;
					rd.addVertexData(chunk);
					rd.uploadData();
				}

				rd.render(worldShader.uniforms["norm"], worldShader.uniforms["t1"], worldShader.uniforms["t2"], camera.position - pos);
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

		glBindBuffer(GL_ARRAY_BUFFER, oqTriangleBufferObject); //bind the buffer we're applying attributes to
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oqTriangleIndicesBufferObject); //bind the buffer we're applying attributes to
		glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0); //tell gl (shader!) how to interpret our vertex data
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

        n += l;
        l *= 2;

        for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
        {
			boost::tuple<int, int, int> &c = visibleChunksVector[i];

			glBeginQuery(GL_ANY_SAMPLES_PASSED, oqIds[c]);

            math::vec3 pos = math::vec3(c.get<0>()*CHUNK_SIZE, c.get<1>()*CHUNK_SIZE, c.get<2>()*CHUNK_SIZE) - math::vec3(0.5f, 0.5f, 0.5f);

			glUniform3fv(oqShader.uniforms["chunkPosition"], 1, &pos[0]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

            glEndQuery(GL_ANY_SAMPLES_PASSED);
		}

		for (size_t i = n; i < (n + l) && i < visibleChunksVector.size(); ++i)
		{
			boost::tuple<int, int, int> &c = visibleChunksVector[i];
			GLuint params;
			glGetQueryObjectuiv(oqIds[c], GL_QUERY_RESULT, &params);
			oqVisibleChunksVector.push_back(params == 1);

		}
		glDisableVertexAttribArray(0);
		glUseProgram(0);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

    //std::cout << "cubes " << cubeCount << " triangles " << trisRendred << " OQ " << occluded  << "/" << visibleChunksVector.size() << std::endl;
}

