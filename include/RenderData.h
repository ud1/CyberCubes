#ifndef RENDERDATA_H
#define RENDERDATA_H

#include <GL/glew.h>
#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <stdint.h>
#include "Math.h"
#include "Chunk.h"

struct Vertex
{
    signed char x, y, z;
    int16_t textureId;
    unsigned char l1, l2, l3, l4;
    unsigned char sl1, sl2, sl3, sl4;
};

struct RenderData
{
    std::vector<Vertex> vertices[6];
    std::vector<int> layerIndices[6];
    int trisRendered;


    GLuint vao = 0;
    GLuint triangleBufferObject = 0;

    void addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk);
    void uploadData();
    size_t getVideoMemUse() const;
    void render(GLint normLocation, GLint t1Location, GLint t2Location, const math::vec3 &eye);
    void addVertexData(const Chunk &chunk);

    template<bool sun>
    float lightAt(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
};

#endif // RENDERDATA_H
