#include "WorldProvider.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <libnoise/noise.h>
#include <zlib.h>
#include <vector>
#include <sqlite3.h>

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

struct SqlLiteString
{
	char *str = nullptr;
	
	~SqlLiteString()
	{
		sqlite3_free(str);
	}
};

WorldProvider::WorldProvider()
{
	db = nullptr;
	selectChunkDataStmt = nullptr;
	insertChunkDataStmt = nullptr;
	selectChunkSLPStmt = nullptr;
	insertChunkSLPStmt = nullptr;
	int rc = sqlite3_open("world/world.db", &db);
	if(rc)
	{
		std::cout << "Open world database error" << std::endl;
		return;
	}
	
	/* Create SQL statement */
	{
		const char *sql = "CREATE TABLE IF NOT EXISTS CHUNK("  \
			"X INT NOT NULL," \
			"Y INT NOT NULL," \
			"Z INT NOT NULL," \
			"DATA BLOB NOT NULL," \
			"PRIMARY KEY (X, Y, Z));";

		/* Execute SQL statement */
		SqlLiteString errMsg;
		rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg.str);

		if (rc != SQLITE_OK)
			std::cout << "Create CHUNK table error: " << errMsg.str;
	}
	
	{
		const char *sql = "CREATE TABLE IF NOT EXISTS CHUNK_SLP("  \
			"X INT NOT NULL," \
			"Y INT NOT NULL," \
			"Z INT NOT NULL," \
			"DATA BLOB NOT NULL," \
			"PRIMARY KEY (X, Y, Z));";

		/* Execute SQL statement */
		SqlLiteString errMsg;
		rc = sqlite3_exec(db, sql, nullptr, 0, &errMsg.str);

		if (rc != SQLITE_OK)
			std::cout << "Create CHUNK_SLP table error: " << errMsg.str;
	}
	
	rc = sqlite3_prepare_v2(db, "SELECT DATA FROM CHUNK WHERE X = ?1 AND Y = ?2 AND Z = ?3", -1, &selectChunkDataStmt, nullptr);
	if (rc != SQLITE_OK)
		std::cout << "Create SELECT chunk data prepared statement failed: " << rc << std::endl;
	
	rc = sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO CHUNK(X, Y, Z, DATA) VALUES(?1, ?2, ?3, ?4)", -1, &insertChunkDataStmt, nullptr);
	if (rc != SQLITE_OK)
		std::cout << "Create INSERT chunk data prepared statement failed: " << rc << std::endl;
	
	
	rc = sqlite3_prepare_v2(db, "SELECT DATA FROM CHUNK_SLP WHERE X = ?1 AND Y = ?2 AND Z = ?3", -1, &selectChunkSLPStmt, nullptr);
	if (rc != SQLITE_OK)
		std::cout << "Create SELECT chunk SLP data prepared statement failed: " << rc << std::endl;
	
	rc = sqlite3_prepare_v2(db, "INSERT OR REPLACE INTO CHUNK_SLP(X, Y, Z, DATA) VALUES(?1, ?2, ?3, ?4)", -1, &insertChunkSLPStmt, nullptr);
	if (rc != SQLITE_OK)
		std::cout << "Create INSERT chunk SLP data prepared statement failed: " << rc << std::endl;
}

WorldProvider::~WorldProvider()
{

	sqlite3_finalize(selectChunkDataStmt);
	sqlite3_finalize(insertChunkDataStmt);
	sqlite3_finalize(selectChunkSLPStmt);
	sqlite3_finalize(insertChunkSLPStmt);
	sqlite3_close(db);
}

bool WorldProvider::save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite)
{
	if (chunk.isDummy)
		return false;
	
	if (!insertChunkDataStmt)
		return false;
	
	uLongf outputSize = compressBound(sizeof(chunk.cubes));
	std::vector<Bytef> buffer;
	buffer.resize(outputSize);
	
	int resultCode = compress2(buffer.data(), &outputSize, reinterpret_cast<const Bytef *>(chunk.cubes), sizeof(chunk.cubes), 3);
	bool result = resultCode == Z_OK;
	
	if (!result)
		return false;
	
	sqlite3_bind_int(insertChunkDataStmt, 1, pos.x);
	sqlite3_bind_int(insertChunkDataStmt, 2, pos.y);
	sqlite3_bind_int(insertChunkDataStmt, 3, pos.z);
	sqlite3_bind_blob(insertChunkDataStmt, 4, (const void *) buffer.data(), outputSize, SQLITE_TRANSIENT);
	int rc = sqlite3_step(insertChunkDataStmt);
	
	sqlite3_reset(insertChunkDataStmt);
	
	return rc == SQLITE_DONE;
}

bool WorldProvider::load(Chunk &chunk, const math::ivec3 &pos)
{
	if (!selectChunkDataStmt)
		return false;
	
	sqlite3_bind_int(selectChunkDataStmt, 1, pos.x);
	sqlite3_bind_int(selectChunkDataStmt, 2, pos.y);
	sqlite3_bind_int(selectChunkDataStmt, 3, pos.z);
	int rc = sqlite3_step(selectChunkDataStmt);
	if (rc != SQLITE_ROW)
	{
		sqlite3_reset(selectChunkDataStmt);
		return false;
	}
	
	const void *data = sqlite3_column_blob(selectChunkDataStmt, 0);
	int dataLen = sqlite3_column_bytes(selectChunkDataStmt, 0);
	if (!data)
	{
		sqlite3_reset(selectChunkDataStmt);
		return false;
	}
	
	uLongf destLen = sizeof(chunk.cubes);
	int resultCode = uncompress(reinterpret_cast<Bytef *>(chunk.cubes), &destLen, (const Bytef *) data, dataLen);
	bool result = resultCode == Z_OK && destLen == sizeof(chunk.cubes);
	
	sqlite3_reset(selectChunkDataStmt);
	return result;
}

bool WorldProvider::load(SunLightPropagationLayer &layer, const math::ivec3 &pos)
{
	if (!selectChunkSLPStmt)
		return false;
	
	sqlite3_bind_int(selectChunkSLPStmt, 1, pos.x);
	sqlite3_bind_int(selectChunkSLPStmt, 2, pos.y);
	sqlite3_bind_int(selectChunkSLPStmt, 3, pos.z);
	int rc = sqlite3_step(selectChunkSLPStmt);
	if (rc != SQLITE_ROW)
	{
		sqlite3_reset(selectChunkSLPStmt);
		return false;
	}
	
	const void *data = sqlite3_column_blob(selectChunkSLPStmt, 0);
	int dataLen = sqlite3_column_bytes(selectChunkSLPStmt, 0);
	if (!data)
	{
		sqlite3_reset(selectChunkSLPStmt);
		return false;
	}
	
	uLongf destLen = sizeof(layer.numBlocks);
	int resultCode = uncompress(reinterpret_cast<Bytef *>(layer.numBlocks), &destLen, (const Bytef *) data, dataLen);
	bool result = resultCode == Z_OK && destLen == sizeof(layer.numBlocks);
	
	sqlite3_reset(selectChunkSLPStmt);
	return result;
	
	layer.needToPersist = false;
	layer.isLoaded.store(true);
	return true;
}

bool WorldProvider::save(const SunLightPropagationLayer &layer, const math::ivec3 &pos)
{
	if (!insertChunkSLPStmt)
		return false;
	
	uLongf outputSize = compressBound(sizeof(layer.numBlocks));
	std::vector<Bytef> buffer;
	buffer.resize(outputSize);
	
	int resultCode = compress2(buffer.data(), &outputSize, reinterpret_cast<const Bytef *>(layer.numBlocks), sizeof(layer.numBlocks), 3);
	bool result = resultCode == Z_OK;
	
	if (!result)
		return false;
	
	sqlite3_bind_int(insertChunkSLPStmt, 1, pos.x);
	sqlite3_bind_int(insertChunkSLPStmt, 2, pos.y);
	sqlite3_bind_int(insertChunkSLPStmt, 3, pos.z);
	sqlite3_bind_blob(insertChunkSLPStmt, 4, (const void *) buffer.data(), outputSize, SQLITE_TRANSIENT);
	int rc = sqlite3_step(insertChunkSLPStmt);
	
	sqlite3_reset(insertChunkSLPStmt);

	return rc == SQLITE_DONE;
}

void WorldProvider::fill(Chunk &chunk, const math::ivec3 &chunkCoord)
{
	chunk.needToPersist = false;
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
		chunk.needToPersist = true;
	}

	chunk.recalcBlockCount();
	if (chunk.slpl)
	{
		chunk.computeSunLightPropagationLayer(*chunk.slpl);
		chunk.slpl->needToPersist = true;
	}
	
	chunk.isLoaded.store(true);
}

void WorldProvider::beginTransaction()
{
	SqlLiteString errorMsg;
	sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errorMsg.str);
	
	if (errorMsg.str)
		std::cout << "Begin transaction error: " << errorMsg.str << std::endl;
}

void WorldProvider::endTransaction()
{
	SqlLiteString errorMsg;
	sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, &errorMsg.str);
	
	if (errorMsg.str)
		std::cout << "End transaction error: " << errorMsg.str << std::endl;
}
