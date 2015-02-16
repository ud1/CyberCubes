#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <memory>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <GL/glew.h>
#include <vector>
#include "Math.h"
#include "Frustum.h"
#include "Shader.h"
#include "Camera.h"
#include "WorldProvider.h"

struct Chunk;
struct RenderData;

class World
{
    public:
    typedef boost::tuple<int, int, int> Coord;
    typedef std::unique_ptr<Chunk> PChunk;
    typedef std::unique_ptr<RenderData> PRenderData;

	PChunk solidChunk;

    std::map<Coord, PChunk > chunks;
    std::map<Coord, PRenderData > renderData;
    std::map<Coord, GLuint> oqIds;

    WorldProvider worldProvider;

    GLuint oqVao = 0;
    GLuint oqTriangleBufferObject = 0;
    GLuint oqTriangleIndicesBufferObject = 0;
    Shader oqShader;
    Shader worldShader;
    float dayNightLightCoef;

    void create();
    void render(const Camera &camera);
    float getMaxLightNearPoint(const math::vec3 &v);

    template<bool sun>
    float getMaxLightAtPoint(const math::vec3 &v);

    CubeType getCubeAt(const math::ivec3 &v);

    template<bool sun>
    LightValue *getLightRef(const math::ivec3 &v);

    void updateLight(const std::vector<math::ivec3> &addedBlocks, const std::vector<math::ivec3> &removedBlocks);
    bool getBlock(const math::vec3 &p, const math::vec3 &ray, float len, math::ivec3 &result, math::ivec3 &prev);
    void move(math::BBox &box, const math::vec3 &delta);
    bool putBlock(const math::ivec3 &v, CubeType c);
};

#endif // WORLD_H
