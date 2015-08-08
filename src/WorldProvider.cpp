#include "WorldProvider.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <libnoise/noise.h>
#include <zlib.h>
#include <vector>

namespace
{

class GradientModule : public noise::module::Module
{
public:

	GradientModule() : Module(0) {}
	virtual int GetSourceModuleCount() const
	{
		return 0;
	}

	virtual double GetValue(double x, double y, double z) const
	{
		return (z) / 50.0;
	}
};

}

std::string getFileName(const math::ivec3 &pos)
{
	std::ostringstream oss;
	oss << "world/" << (pos.x > 0 ? "p" : "n") << std::abs(pos.x) << (pos.y > 0 ? "p" : "n") << std::abs(pos.y) << (pos.z > 0 ? "p" : "n") << std::abs(pos.z) << ".wdat";
	return oss.str();
}

std::string getSLFileName(const math::ivec3 &pos)
{
	std::ostringstream oss;
	oss << "world/" << (pos.x > 0 ? "p" : "n") << std::abs(pos.x) << (pos.y > 0 ? "p" : "n") << std::abs(pos.y) << (pos.z > 0 ? "p" : "n") << std::abs(pos.z) << ".sldat";
	return oss.str();
}

bool WorldProvider::save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite)
{
	if (chunk.isDummy)
		return false;

	std::string fileName = getFileName(pos);

	if (!rewrite)
	{
		std::ifstream file(fileName, std::ios::binary);

		if (file)
			return false;
	}

	std::ofstream file(fileName, std::ios::binary);

	if (!file)
		return false;
	
	uLongf outputSize = compressBound(sizeof(chunk.cubes));
	std::vector<Bytef> buffer;
	buffer.resize(outputSize);
	
	int resultCode = compress2(buffer.data(), &outputSize, reinterpret_cast<const Bytef *>(chunk.cubes), sizeof(chunk.cubes), 3);
	bool result = resultCode == Z_OK;
	
	file.write((char *) buffer.data(), outputSize);

	//std::cout << "Save " << fileName << " " << (result ? "OK" : "FAIL") << std::endl;
	return result;
}

bool WorldProvider::load(Chunk &chunk, const math::ivec3 &pos)
{
	std::string fileName = getFileName(pos);
	std::ifstream file(fileName, std::ios::binary);

	if (!file)
		return false;

	file.seekg(0, std::ios_base::end);
	size_t fileSize = file.tellg();
	
	if (!fileSize)
		return false;
	
	file.seekg(0, std::ios_base::beg);
	
	std::vector<Bytef> buffer;
	buffer.resize(fileSize);
	file.read((char *) buffer.data(), fileSize);
	
	uLongf destLen = sizeof(chunk.cubes);
	int resultCode = uncompress(reinterpret_cast<Bytef *>(chunk.cubes), &destLen, buffer.data(), fileSize);
	bool result = resultCode == Z_OK && destLen == sizeof(chunk.cubes);
	
	//std::cout << "Load " << fileName << " " << (result ? "OK" : "FAIL") << std::endl;
	return result;
}

bool WorldProvider::load(SunLightPropagationLayer &layer, const math::ivec3 &pos)
{
	std::string fileName = getSLFileName(pos);
	std::ifstream file(fileName, std::ios::binary);

	if (!file)
		return false;
	
	file.read((char *) layer.numBlocks, sizeof(layer.numBlocks));
	layer.isLoaded.store(true);
	return true;
}

bool WorldProvider::save(const SunLightPropagationLayer &layer, const math::ivec3 &pos)
{
	std::string fileName = getSLFileName(pos);
	std::ofstream file(fileName, std::ios::binary);

	if (!file)
		return false;
	
	file.write((const char *) layer.numBlocks, sizeof(layer.numBlocks));
	return true;
}

void WorldProvider::fill(Chunk &chunk, const math::ivec3 &chunkCoord)
{

	if (!load(chunk, chunkCoord))
	{
		GradientModule gradientModule;
		noise::module::Turbulence turbulence;
		turbulence.SetSourceModule(0, gradientModule);
		turbulence.SetFrequency(0.03);
		turbulence.SetPower(40.0);

		for (unsigned int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (unsigned int y = 0; y < CHUNK_SIZE; ++y)
			{
				for (unsigned int z = 0; z < CHUNK_SIZE; ++z)
				{
					math::ivec3 p(x, y, z);
					math::vec3 pos = i2f(chunkCoord * CHUNK_SIZE_I + p);

					if (pos.z > 256)
						continue;

					double v = turbulence.GetValue(pos.x, pos.y, pos.z);

					if ((v < 1.0 || v > 1.8) && (v < 2.5 || v > 3.0) && (v < 5.0 || v > 5.2) && (v < 7.0 || v > 7.2)
							&& (v < 9.0 || v > 9.5) && (v < 12.0 || v > 12.3)
					   )
					{
						int type = ((float) rand() / (float) RAND_MAX > (pos.z / 200.0)) ? 1 : 2;

						// TODO deterministic random
						if ((float) rand() / (float) RAND_MAX < 0.1)
							type = 1;
						else if ((float) rand() / (float) RAND_MAX > 0.9)
							type = 2;

						chunk.put(p, type);
					}
				}
			}
		}
	}

	chunk.recalcBlockCount();
	if (chunk.slpl)
		chunk.computeSunLightPropagationLayer(*chunk.slpl);
	
	chunk.isLoaded.store(true);
}

