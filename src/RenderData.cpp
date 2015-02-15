#include <iostream>

#include "RenderData.h"

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

							count = (count + 0.5) / 1.07142857142857142857;

							lblock.multiplier = 1.0f / count / (float)MAX_LIGHT * 255.0;
						}
					}
				}
			}
        }
	}

} lightCoefs;

void RenderData::addFace(const math::ivec3 &pos, int textureId, Dir dir, const Chunk &chunk)
{
	Vertex vertex;
	vertex.x = pos.x;
	vertex.y = pos.y;
	vertex.z = pos.z;
	vertex.textureId = textureId;


	if (dir == Dir::XN)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x - 1, pos.y, pos.z));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x - 1, pos.y, pos.z));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x - 1, pos.y, pos.z));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x - 1, pos.y, pos.z));
	}
	else if (dir == Dir::XP)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x + 1, pos.y, pos.z));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x + 1, pos.y, pos.z));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x + 1, pos.y, pos.z));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x + 1, pos.y, pos.z));
	}
	else if (dir == Dir::YN)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x, pos.y - 1, pos.z));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x, pos.y - 1, pos.z));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y - 1, pos.z));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y - 1, pos.z));
	}
	else if (dir == Dir::YP)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y + 1, pos.z));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y + 1, pos.z));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y + 1, pos.z));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y + 1, pos.z));

	}
	else if (dir == Dir::ZN)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z), math::ivec3(pos.x, pos.y, pos.z - 1));
	}
	else if (dir == Dir::ZP)
	{
		vertex.l1 = lightAt(chunk, math::ivec3(pos.x, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
        vertex.l2 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
        vertex.l3 = lightAt(chunk, math::ivec3(pos.x, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
        vertex.l4 = lightAt(chunk, math::ivec3(pos.x + 1, pos.y + 1, pos.z + 1), math::ivec3(pos.x, pos.y, pos.z + 1));
	}

	/*if (dir == XN)
	{
        vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x - 1, pos.y, pos.z)) / (float)MAX_LIGHT * 255.0;
	}
	else if (dir == XP)
	{
		vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x + 1, pos.y, pos.z)) / (float)MAX_LIGHT * 255.0;
	}
	else if (dir == YN)
	{
		vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x, pos.y - 1, pos.z)) / (float)MAX_LIGHT * 255.0;
	}
	else if (dir == YP)
	{
		vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x, pos.y + 1, pos.z)) / (float)MAX_LIGHT * 255.0;
	}
	else if (dir == ZN)
	{
		vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x, pos.y, pos.z - 1)) / (float)MAX_LIGHT * 255.0;
	}
	else if (dir == ZP)
	{
		vertex.l1 = vertex.l2 = vertex.l3 = vertex.l4 = chunk.lightAt(math::ivec3(pos.x, pos.y, pos.z + 1)) / (float)MAX_LIGHT * 255.0;
	}*/

	vertices[(size_t)dir].push_back(vertex);
}

float RenderData::lightAt(const Chunk &chunk, const math::ivec3 &pos, const math::ivec3 &from)
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
				LightValue l = chunk.lightAt(math::ivec3(pos.x - i, pos.y - j, pos.z - k));
				value[1 - i][1 - j][1 - k] = l > 0;
			}
		}
    }

    int p = 0;
    for (int c = 0; c < 8; ++c)
	{
		p |= (value[(c & 4) >> 2][(c & 2) >> 1][c & 1] ? 1 : 0) << c;
	}

	LightCoefsBlock &lblock = lightCoefs.blocks[fk * 256 + p];

	float result = 0.0f;
	for (int i = 0; i < 2; ++i)
    {
		for (int j = 0; j < 2; ++j)
		{
			for (int k = 0; k < 2; ++k)
			{
				if (lblock.resultValue[1 - i][1 - j][1 - k])
				{
					LightValue l = chunk.lightAt(math::ivec3(pos.x - i, pos.y - j, pos.z - k));
					result += l;
				}
			}
		}
    }

    return result * lblock.multiplier;
}

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
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_SHORT, GL_FALSE, sizeof(Vertex), &((Vertex *)NULL)->textureId);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), &((Vertex *)NULL)->l1);
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

void RenderData::render(GLint normLocation, GLint t1Location, GLint t2Location, const math::vec3 &eye)
{
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
