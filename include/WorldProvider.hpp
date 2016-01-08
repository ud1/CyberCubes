#ifndef WORLDPROVIDER_H
#define WORLDPROVIDER_H

#include "Chunk.hpp"
#include "Math.hpp"

struct sqlite3;
struct sqlite3_stmt;

class WorldProvider
{
public:
	WorldProvider();
	~WorldProvider();
	
	bool save(const Chunk &chunk, const math::ivec3 &pos, bool rewrite);
	bool load(Chunk &chunk, const math::ivec3 &pos);
	bool load(SunLightPropagationLayer &layer, const math::ivec3 &pos);
	bool save(const SunLightPropagationLayer &layer, const math::ivec3 &pos);

	void fill(Chunk &chunk, const math::ivec3 &pos);
	
	void beginTransaction();
	void endTransaction();
private:
	sqlite3 *db;
	sqlite3_stmt *selectChunkDataStmt, *insertChunkDataStmt;
	sqlite3_stmt *selectChunkBlockDataStmt, *insertChunkBlockDataStmt, *deleteChunkBlockDataStmt;
	sqlite3_stmt *selectChunkSLPStmt, *insertChunkSLPStmt;
};

#endif // WORLDPROVIDER_H
