#include <iostream>

#include "RenderData.hpp"
#include "colorTransform.hpp"
#include "Block.hpp"
#include "blockType.hpp"

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

							if (count < 8)
							{
								constexpr double d = 1.0;
								count = (count + d) / (1.0 + d / 7.0);
							}

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
	
	if (indexBufferObject != 0)
		glDeleteBuffers(1, &indexBufferObject);
	
	if (opaqueVao != 0)
		glDeleteVertexArrays(1, &opaqueVao);
	
	if (nonOpaqueVao != 0)
		glDeleteVertexArrays(1, &nonOpaqueVao);
}

size_t RenderData::addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk, unsigned char quadRotation, bool opaque)
{
	Vertex vertex;
	vertex.x = pos.x;
	vertex.y = pos.y;
	vertex.z = pos.z;
	vertex.textureId = textureId + 1;
	vertex.normalIndex = (unsigned char) dir;
	vertex.rotation = quadRotation;

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

	if (opaque)
	{
		opaqueVertices[(size_t)dir].push_back(vertex);
		return 0;
	}
	else
	{
		nonOpaqueVertices.push_back(vertex);
		return nonOpaqueVertices.size() - 1;
	}
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
				CubeType c = chunk.cubeAt(math::ivec3(pos.x - i, pos.y - j, pos.z - k));
				value[1 - i][1 - j][1 - k] = isLightTransparent(c);
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
	if (opaqueVao == 0)
		glGenVertexArrays(1, &opaqueVao);
	
	if (nonOpaqueVao == 0)
		glGenVertexArrays(1, &nonOpaqueVao);

	glBindVertexArray(0);

	if (triangleBufferObject == 0)
		glGenBuffers(1, &triangleBufferObject);
	
	if (indexBufferObject == 0)
		glGenBuffers(1, &indexBufferObject);

	int size = 0;
	for (int i = 0; i < 6; ++i)
	{
		size += opaqueVertices[i].size();
	}
	size += nonOpaqueVertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), NULL, GL_STATIC_DRAW); //formatting the data for the buffer
	GLintptr offset = 0;
	for (int i = 0; i < 6; ++i)
	{
		if (opaqueVertices[i].size() > 0)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, opaqueVertices[i].size() * sizeof(Vertex), &opaqueVertices[i][0]);
			offset += opaqueVertices[i].size() * sizeof(Vertex);
		}
	}
	
	if (nonOpaqueVertices.size() > 0)
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset, nonOpaqueVertices.size() * sizeof(Vertex), &nonOpaqueVertices[0]);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		int count = 0;
		for (int i = 0; i < 8; ++i)
		{
			count += nonOpaqueIndices[i].size();
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint16_t), NULL, GL_STATIC_DRAW);
		
		offset = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (nonOpaqueIndices[i].size())
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, nonOpaqueIndices[i].size() * sizeof(uint16_t), &nonOpaqueIndices[i][0]);
			offset += nonOpaqueIndices[i].size() * sizeof(uint16_t);
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		glBindVertexArray(i == 0 ? opaqueVao : nonOpaqueVao);
		
		glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), 0);
		
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), &((Vertex *)nullptr)->normalIndex);
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->colorH);
		
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 2, GL_UNSIGNED_BYTE, sizeof(Vertex), &((Vertex *)nullptr)->rotation);
		
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_SHORT, sizeof(Vertex), &((Vertex *)nullptr)->textureId);
		
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->l1);
		
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)nullptr)->sl1);
		
		if (i == 1)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		}
	}
	
	glBindVertexArray(0);

	std::cout << "vertices " << size << std::endl;
	printf("glError: %d\n", glGetError());
}

size_t RenderData::getVideoMemUse() const
{
	int size = 0;
	for (int i = 0; i < 6; ++i)
	{
		size += opaqueVertices[i].size();
	}
	size += nonOpaqueVertices.size();
	return size * sizeof(Vertex);
}

typedef struct {
	GLuint count;
	GLuint primCount;
	GLuint first;
	GLuint baseInstance;
} DrawArraysIndirectCommand;

void RenderData::render(GLint clipDirLocation, const math::vec3 &eye, Tick tick, bool opaque)
{
	_renderTick = tick;
	
	math::ivec3 eyePos = math::ivec3(std::floor(eye.x + 0.5f), std::floor(eye.y + 0.5f), std::floor(eye.z + 0.5f));
	eyePos = math::clamp(eyePos, math::ivec3(-1, -1, -1), math::ivec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE)) + math::ivec3(1, 1, 1);

	glBindVertexArray(opaque ? opaqueVao : nonOpaqueVao);

	int indexOffset = 0;
	
	GLint mdFirst[6];
	GLsizei mdCount[6];
	
	int total = 0;
	int k = 0;
	for (int i = 0 ; i < 6; ++i)
	{

		if (opaque && !opaqueVertices[i].empty())
		{
			int delta;
		
			if (i == (int) Dir::XN)
			{
				delta = eyePos.x;
			}
			else if (i == (int) Dir::XP)
			{
				delta = CHUNK_SIZE + 1 - eyePos.x;
			}
			else if (i == (int) Dir::YN)
			{
				delta = eyePos.y;
			}
			else if (i == (int) Dir::YP)
			{
				delta = CHUNK_SIZE + 1 - eyePos.y;
			}
			else if (i == (int) Dir::ZN)
			{
				delta = eyePos.z;
			}
			else if (i == (int) Dir::ZP)
			{
				delta = CHUNK_SIZE + 1 - eyePos.z;
			}
			
			if (delta < (int) CHUNK_SIZE)
			{
				GLsizei count = opaqueVertices[i].size() - opaqueLayerIndices[i][delta];
				total += count;
				if (count)
				{
					mdFirst[k] = indexOffset + opaqueLayerIndices[i][delta];
					mdCount[k] = count;
					++k;
				}
			}
		}
		indexOffset += opaqueVertices[i].size();
	}

	if (k)
		glMultiDrawArrays(GL_POINTS, mdFirst, mdCount, k);
	
	if (!opaque && !nonOpaqueVertices.empty())
	{
		for (int k = 0; k < 8; ++k)
		{
			bool xp = (k & 1);
			bool yp = (k & 2);
			bool zp = (k & 4);
			
			if (xp && eye.x < -0.5f)
				continue;
			if (yp && eye.y < -0.5f)
				continue;
			if (zp && eye.z < -0.5f)
				continue;
			if (!xp && eye.x > CHUNK_SIZE_F - 0.5f)
				continue;
			if (!yp && eye.y > CHUNK_SIZE_F - 0.5f)
				continue;
			if (!zp && eye.z > CHUNK_SIZE_F - 0.5f)
				continue;
			
			math::vec3 clipDir = math::vec3(xp ? -1 : 1, yp ? -1 : 1, zp ? -1 : 1);
			
			int indexStart = 0;
			for (int i = 0; i < k; ++i)
			{
				indexStart += nonOpaqueIndices[i].size();
			}
			
			GLsizei count = nonOpaqueIndices[k].size();
			total += count;
			if (count)
			{
				if (clipDirLocation >= 0)
					glUniform3fv(clipDirLocation, 1, &clipDir[0]);
			
				glDrawElementsBaseVertex(GL_POINTS, nonOpaqueIndices[k].size(), GL_UNSIGNED_SHORT, (uint16_t *) nullptr + indexStart, indexOffset);
			}
		}
	}

	trisRendered = total * 2;

	glBindVertexArray(0);
}

int getTextureId(CubeType cubeType, Dir dir, int rot)
{
	int textureId = 0;
	Block *block = Block::get(cubeType);
	if (block)
		textureId = block->textureId[(int) RotationMatrices::getInstance().invRotate(dir, rot)];
	
	return textureId;
}

void RenderData::addVertexData(const Chunk &chunk)
{
	for (int i = 0 ; i < 6; ++i)
	{
		opaqueLayerIndices[i].clear();
		opaqueVertices[i].clear();
		opaqueLayerIndices[i].resize(CHUNK_SIZE);
	}
	
	for (int i = 0; i < 8; ++i)
		nonOpaqueIndices[i].clear();
	
	nonOpaqueVertices.clear();
	std::vector<int> facesInds;
	facesInds.resize(CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE*6, -1);

	bool nonOpaqueExists = false;
	for (unsigned int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int l = 0 ; l < 6; ++l)
		{
			opaqueLayerIndices[l][i] = opaqueVertices[l].size();
		}

		for (unsigned int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (unsigned int k = 0; k < CHUNK_SIZE; ++k)
			{
				size_t index;
				bool opaque;
				
				math::ivec3 p = math::ivec3(i, j, k);
				
				if (chunk.hasEdge(p, Dir::XN))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::XN, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::XN, rot), Dir::XN, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 0] = index;
						nonOpaqueExists = true;
					}
				}

				p = math::ivec3(CHUNK_SIZE - 1 - i, j, k);
				if (chunk.hasEdge(p, Dir::XP))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::XP, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::XP, rot), Dir::XP, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 1] = index;
						nonOpaqueExists = true;
					}
				}
				
				p = math::ivec3(j, i, k);
				if (chunk.hasEdge(p, Dir::YN))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::YN, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::YN, rot), Dir::YN, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 2] = index;
						nonOpaqueExists = true;
					}
				}
				
				p = math::ivec3(j, CHUNK_SIZE - 1 - i, k);
				if (chunk.hasEdge(p, Dir::YP))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::YP, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::YP, rot), Dir::YP, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 3] = index;
						nonOpaqueExists = true;
					}
				}
				
				p = math::ivec3(j, k, i);
				if (chunk.hasEdge(p, Dir::ZN))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::ZN, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::ZN, rot), Dir::ZN, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 4] = index;
						nonOpaqueExists = true;
					}
				}
				
				p = math::ivec3(j, k, CHUNK_SIZE - 1 - i);
				if (chunk.hasEdge(p, Dir::ZP))
				{
					int rot = Chunk::decodeRotation(chunk.getBlockData(p));
					unsigned char quadRot = RotationMatrices::getInstance().getQuadRotation(Dir::ZP, rot);
					index = addFace(p, getTextureId(chunk.rawCubeAt(p), Dir::ZP, rot), Dir::ZP, chunk, quadRot, opaque = isOpaque(chunk.rawCubeAt(p)));
					if (!opaque)
					{
						facesInds[(((p.x * CHUNK_SIZE) + p.y) * CHUNK_SIZE + p.z)*6 + 5] = index;
						nonOpaqueExists = true;
					}
				}
			}
		}
	}
	
	if (nonOpaqueExists)
	{
		for (unsigned int i = 0; i < CHUNK_SIZE; ++i)
		{
			for (unsigned int j = 0; j < CHUNK_SIZE; ++j)
			{
				for (unsigned int k = 0; k < CHUNK_SIZE; ++k)
				{
					for (int d = 0; d < 8; ++d)
					{
						bool xp = (d & 1);
						bool yp = (d & 2);
						bool zp = (d & 4);
						
						int x = xp ? i : CHUNK_SIZE - 1 - i;
						int y = yp ? j : CHUNK_SIZE - 1 - j;
						int z = zp ? k : CHUNK_SIZE - 1 - k;
						int ind = (((x * CHUNK_SIZE) + y) * CHUNK_SIZE + z)*6;
						
						if (xp && facesInds[ind + 0] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 0]);
						else if (!xp && facesInds[ind + 1] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 1]);
						
						if (yp && facesInds[ind + 2] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 2]);
						else if (!yp && facesInds[ind + 3] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 3]);
						
						if (zp && facesInds[ind + 4] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 4]);
						else if (!zp && facesInds[ind + 5] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 5]);
						
						if (xp && facesInds[ind + 1] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 1]);
						else if (!xp && facesInds[ind + 0] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 0]);
						
						if (yp && facesInds[ind + 3] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 3]);
						else if (!yp && facesInds[ind + 2] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 2]);
						
						if (zp && facesInds[ind + 5] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 5]);
						else if (!zp && facesInds[ind + 4] >= 0)
							nonOpaqueIndices[d].push_back(facesInds[ind + 4]);
					}
				}
			}
		}
		
		for (int i = 0; i < 8; ++i)
		{
			if (nonOpaqueIndices[i].size() < 10)
			{
				for (int j = 0; j < nonOpaqueIndices[i].size(); ++j)
				{
					std::cout << "INT " << j << " " << nonOpaqueIndices[i][j] << std::endl;
				}
			}
		}
	}
}
