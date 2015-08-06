#include "WorldProvider.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <libnoise/noise.h>

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
	oss << "world/" << (pos.x > 0 ? "p" : "n") << pos.x << (pos.y > 0 ? "p" : "n") << pos.y << (pos.z > 0 ? "p" : "n") << pos.z << ".wdat";
	return oss.str();
}

bool WorldProvider::save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite)
{
	if (chunk.isDummy)
		return false;

	std::string fileName = getFileName(pos);

	if (!rewrite)
	{
		std::ifstream file(getFileName(pos), std::ios::binary);

		if (file)
			return false;
	}

	std::ofstream file(getFileName(pos), std::ios::binary);

	file.write((char *) chunk.cubes, sizeof(chunk.cubes));
	//file.write((char *) chunk.light, sizeof(chunk.light));

	return true;
}

bool WorldProvider::load(Chunk &chunk, const math::ivec3 &pos)
{
	std::ifstream file(getFileName(pos), std::ios::binary);

	if (!file)
		return false;

	file.read((char *) chunk.cubes, sizeof(chunk.cubes));
	//file.read((char *) chunk.light, sizeof(chunk.light));

	std::cout << "Load OK " << getFileName(pos) << std::endl;
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
	
	chunk.isLoaded.store(true);
}

