#ifndef RENDERDATA_H
#define RENDERDATA_H

#include <GL/glew.h>
#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <stdint.h>
#include "Math.hpp"
#include "Chunk.hpp"

struct Vertex
{
	signed char x, y, z;
	unsigned char colorH, colorS;
	int16_t textureId;
	unsigned char l1, l2, l3, l4;
	unsigned char sl1, sl2, sl3, sl4;
};

struct RenderData
{
	~RenderData();
	
	std::vector<Vertex> opaqueVertices[6];
	std::vector<int> opaqueLayerIndices[6];
	std::vector<Vertex> nonOpaqueVertices[6];
	std::vector<int> nonOpaqueLayerIndices[6];
	int trisRendered;
	

	GLuint vao = 0;
	GLuint triangleBufferObject = 0;

	void addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk, bool opaque);
	void uploadData();
	size_t getVideoMemUse() const;
	void render(GLint normLocation, GLint t1Location, GLint t2Location, const math::vec3 &eye, Tick tick, bool opaque);
	void addVertexData(const Chunk &chunk);

	template<LightType lt>
	LightValF<lt> lightAt(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
	
	Tick getLastRenderTick() const
	{
		return _renderTick;
	}
	
private:
	Tick _renderTick = 0;
};

#endif // RENDERDATA_H
