#include <iostream>

#include "RenderData.h"
#include "colorTransform.h"

struct LightCoefsBlock
{
    bool value[2][2][2];
    bool resultValue[2][2][2];
    float multiplier;
};

struct LightCoefs
{
	LightCoefsBlock blocks[256*8];

	LightCoefs()
	{
		for (int fx = 0; fx < 2; ++fx)
		{
			for (int fy = 0; fy < 2; ++fy)
			{
				for (int fz = 0; fz < 2; ++fz)
				{
					int fk = (fx * 2 + fy) * 2 + fz;

					for (int p = 0; p < 256; ++p)
					{
						LightCoefsBlock &lblock = blocks[fk * 256 + p];

						for (int c = 0; c < 8; ++c)
						{
							lblock.value[(c & 4) >> 2][(c & 2) >> 1][c & 1] = (p & (1 << c)) > 0;
							lblock.resultValue[(c & 4) >> 2][(c & 2) >> 1][c & 1] = false;
						}

						lblock.multiplier = 0.0f;

						if ( (lblock.resultValue[fx][fy][fz] = lblock.value[fx][fy][fz]) )
						{
							for (int iter = 0; iter < 4; ++iter)
							{
								for (int c = 0; c < 8; ++c)
								{
									if (lblock.value[(c & 4) >> 2][(c & 2) >> 1][c & 1] && !lblock.resultValue[(c & 4) >> 2][(c & 2) >> 1][c & 1])
									{
										lblock.resultValue[(c & 4) >> 2][(c & 2) >> 1][c & 1] =
											lblock.resultValue[1 - ((c & 4) >> 2)][(c & 2) >> 1][c & 1] ||
											lblock.resultValue[(c & 4) >> 2][1 - ((c & 2) >> 1)][c & 1] ||
											lblock.resultValue[(c & 4) >> 2][(c & 2) >> 1][1 - (c & 1)];
									}
								}
							}

							float count = 0.0f;
							for (int c = 0; c < 8; ++c)
							{
								if (lblock.resultValue[(c & 4) >> 2][(c & 2) >> 1][c & 1])
									++count;
							}

							constexpr double d = 1.0;
							count = (count + d) / (1.0 + d / 7.0);

							lblock.multiplier = 1.0f / count / (float)MAX_LIGHT * 255.0;
						}
					}
				}
			}
        }
	}

} lightCoefs;

float lightCoef(float l)
{
	return std::pow(0.02f, 1.0f - l) - 0.02*(1.0f - l);
}

void colorAt(const Chunk &chunk, const math::ivec3 &pos, unsigned char &colorH, unsigned char &colorS)
{
	math::ivec4 c = chunk.lightAt<LIGHT_SUNRGBA>(pos);
	float r = c.y / MAX_LIGHT_F;
	float g = c.z / MAX_LIGHT_F;
	float b = c.w / MAX_LIGHT_F;

	float h, s, i;
	rgb_hsi_norm(lightCoef(r), lightCoef(g), lightCoef(b), h, s, i);
	colorH = h * 255.0f;
	colorS = s * 255.0f;
}

RenderData::~RenderData()
{
	if (triangleBufferObject != 0)
		glDeleteBuffers(1, &triangleBufferObject);
	
	if (vao != 0)
		glDeleteVertexArrays(1, &vao);
}

void RenderData::addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk)
{
	Vertex vertex;
	vertex.x = pos.x;
	vertex.y = pos.y;
	vertex.z = pos.z;
	vertex.textureId = textureId;


	if (dir == Dir::XN)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x - 1, pos.y, pos.z));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x - 1, pos.y, pos.z));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x - 1, pos.y, pos.z));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x - 1, pos.y, pos.z));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x - 1, pos.y, pos.z), vertex.colorH, vertex.colorS);
	}
	else if (dir == Dir::XP)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x + 1, pos.y, pos.z));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x + 1, pos.y, pos.z));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x + 1, pos.y, pos.z));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x + 1, pos.y, pos.z));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), vertex.colorH, vertex.colorS);
	}
	else if (dir == Dir::YN)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x, pos.y - 1, pos.z));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x, pos.y - 1, pos.z));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y - 1, pos.z));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y - 1, pos.z));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x, pos.y - 1, pos.z), vertex.colorH, vertex.colorS);
	}
	else if (dir == Dir::YP)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y + 1, pos.z));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y + 1, pos.z));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y + 1, pos.z));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y + 1, pos.z));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), vertex.colorH, vertex.colorS);
	}
	else if (dir == Dir::ZN)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x, pos.y, pos.z - 1), vertex.colorH, vertex.colorS);
	}
	else if (dir == Dir::ZP)
	{
		math::vec4 l1 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
		math::vec4 l2 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
		math::vec4 l3 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
		math::vec4 l4 = lightAt<LIGHT_SUNRGBA>(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));

		vertex.sl1 = l1.x;
		vertex.sl2 = l2.x;
		vertex.sl3 = l3.x;
		vertex.sl4 = l4.x;

		vertex.l1 = (l1.y + l1.z + l1.w) / 3.0f;
		vertex.l2 = (l2.y + l2.z + l2.w) / 3.0f;
		vertex.l3 = (l3.y + l3.z + l3.w) / 3.0f;
		vertex.l4 = (l4.y + l4.z + l4.w) / 3.0f;

		colorAt(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), vertex.colorH, vertex.colorS);
	}

	vertices[(size_t)dir].push_back(vertex);
}

template<LightType lt>
LightValF<lt> RenderData::lightAt(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from)
{
	math::ivec3 fromRel = from - pos + math::ivec3(1, 1, 1);
	int fk = (fromRel.x * 2 + fromRel.y) * 2 + fromRel.z;

	bool value[2][2][2];

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				LightVal<lt> l = chunk.lightAt<lt>(math::ivec3(pos.x - i, pos.y - j, pos.z - k));
				value[1 - i][1 - j][1 - k] = isNotNull< LightVal<lt> >(l);
			}
		}
	}

	int p = 0;
	for (int c = 0; c < 8; ++c)
	{
		p |= (value[(c & 4) >> 2][(c & 2) >> 1][c & 1] ? 1 : 0) << c;
	}

	LightCoefsBlock &lblock = lightCoefs.blocks[fk * 256 + p];

	LightValF<lt> result = defVal< LightValF<lt> >();
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				if (lblock.resultValue[1 - i][1 - j][1 - k])
				{
					LightVal<lt> l = chunk.lightAt<lt>(math::ivec3(pos.x - i, pos.y - j, pos.z - k));
					result += l;
				}
			}
		}
	}
	
	return result * lblock.multiplier;
}

template float RenderData::lightAt<LIGHT_SUN>(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
template float RenderData::lightAt<LIGHT_R>(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
template float RenderData::lightAt<LIGHT_G>(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
template float RenderData::lightAt<LIGHT_B>(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);
template LightValF<LIGHT_SUNRGBA> RenderData::lightAt<LIGHT_SUNRGBA>(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from);

void RenderData::uploadData()
{
	if (vao == 0)
		glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	if (triangleBufferObject == 0)
		glGenBuffers(1, &triangleBufferObject);

	int size = vertices[0].size()
               + vertices[1].size()
               + vertices[2].size()
               + vertices[3].size()
               + vertices[4].size()
               + vertices[5].size();

	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), NULL, GL_STATIC_DRAW); //formatting the data for the buffer
	GLintptr offset = 0;
	for (int i = 0; i < 6; ++i)
	{
		if (vertices[i].size() > 0)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, vertices[i].size() * sizeof(Vertex), &vertices[i][0]);
			offset += vertices[i].size() * sizeof(Vertex);
		}
	}


	glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), 0); //tell gl (shader!) how to interpret our vertex data
	glEnableVertexAttribArray(1); //0 is our index, refer to "location = 0" in the vertex shader
	glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->colorH); //tell gl (shader!) how to interpret our vertex data
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_SHORT, GL_FALSE, sizeof(Vertex), &((Vertex *)nullptr)->textureId);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->l1);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->sl1);
	glBindVertexArray(0);

	std::cout << "vertices " << size << std::endl;
	printf("glError: %d\n", glGetError());
}

size_t RenderData::getVideoMemUse() const
{
	int size = vertices[0].size()
               + vertices[1].size()
               + vertices[2].size()
               + vertices[3].size()
               + vertices[4].size()
               + vertices[5].size();
	return size * sizeof(Vertex);
}

void RenderData::render(GLint normLocation, GLint t1Location, GLint t2Location, const math::vec3 &eye, Tick tick)
{
	_renderTick = tick;
	
	math::ivec3 eyePos = math::ivec3(std::floor(eye.x + 0.5f), std::floor(eye.y + 0.5f), std::floor(eye.z + 0.5f));
	eyePos = math::clamp(eyePos, math::ivec3(-1, -1, -1), math::ivec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE)) + math::ivec3(1, 1, 1);

	glBindVertexArray(vao);

	int indexOffset = 0;
	int total = 0;
	for (int i = 0 ; i < 6; ++i)
	{

		if (!vertices[i].empty())
		{
			int delta;

			if (i == (int) Dir::XN)
			{
				glUniform3f(normLocation, -1.0f, 0.0f, 0.0f);
				glUniform3f(t1Location, 0.0f, 1.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 0.0f, 1.0f);
				delta = eyePos.x;
			}
			else if (i == (int) Dir::XP)
			{
				glUniform3f(normLocation, 1.0f, 0.0f, 0.0f);
				glUniform3f(t1Location, 0.0f, 1.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 0.0f, 1.0f);
				delta = CHUNK_SIZE + 1 - eyePos.x;
			}
			else if (i == (int) Dir::YN)
			{
				glUniform3f(normLocation, 0.0f, -1.0f, 0.0f);
				glUniform3f(t1Location, 1.0f, 0.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 0.0f, 1.0f);
				delta = eyePos.y;
			}
			else if (i == (int) Dir::YP)
			{
				glUniform3f(normLocation, 0.0f, 1.0f, 0.0f);
				glUniform3f(t1Location, 1.0f, 0.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 0.0f, 1.0f);
				delta = CHUNK_SIZE + 1 - eyePos.y;
			}
			else if (i == (int) Dir::ZN)
			{
				glUniform3f(normLocation, 0.0f, 0.0f, -1.0f);
				glUniform3f(t1Location, 1.0f, 0.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 1.0f, 0.0f);
				delta = eyePos.z;
			}
			else if (i == (int) Dir::ZP)
			{
				glUniform3f(normLocation, 0.0f, 0.0f, 1.0f);
				glUniform3f(t1Location, 1.0f, 0.0f, 0.0f);
				glUniform3f(t2Location, 0.0f, 1.0f, 0.0f);
				delta = CHUNK_SIZE + 1 - eyePos.z;
			}

			if (delta < (int) CHUNK_SIZE)
			{
				GLsizei count = vertices[i].size() - layerIndices[i][delta];
				total += count;
				if (count)
				glDrawArrays(GL_POINTS, indexOffset + layerIndices[i][delta], count);
			}

			indexOffset += vertices[i].size();
		}
	}

	trisRendered = total * 2;

	glBindVertexArray(0);
}

void RenderData::addVertexData(const Chunk &chunk)
{
	for (int i = 0 ; i < 6; ++i)
	{
		layerIndices[i].clear();
		vertices[i].clear();
		layerIndices[i].resize(CHUNK_SIZE);
	}

	for (unsigned int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int l = 0 ; l < 6; ++l)
		{
			layerIndices[l][i] = vertices[l].size();
		}

		for (unsigned int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (unsigned int k = 0; k < CHUNK_SIZE; ++k)
			{
				math::ivec3 p = math::ivec3(i, j, k);
				if (chunk.hasEdge(p, Dir::XN))
					addFace(p, chunk.cubeAt(p), Dir::XN, *&chunk);

				p = math::ivec3(CHUNK_SIZE - 1 - i, j, k);
				if (chunk.hasEdge(p, Dir::XP))
					addFace(p, chunk.cubeAt(p), Dir::XP, *&chunk);

				p = math::ivec3(j, i, k);
				if (chunk.hasEdge(p, Dir::YN))
					addFace(p, chunk.cubeAt(p), Dir::YN, *&chunk);

				p = math::ivec3(j, CHUNK_SIZE - 1 - i, k);
				if (chunk.hasEdge(p, Dir::YP))
					addFace(p, chunk.cubeAt(p), Dir::YP, *&chunk);

				p = math::ivec3(j, k, i);
				if (chunk.hasEdge(p, Dir::ZN))
					addFace(p, chunk.cubeAt(p), Dir::ZN, *&chunk);

				p = math::ivec3(j, k, CHUNK_SIZE - 1 - i);
				if (chunk.hasEdge(p, Dir::ZP))
					addFace(p, chunk.cubeAt(p), Dir::ZP, *&chunk);
			}
		}
	}
}
