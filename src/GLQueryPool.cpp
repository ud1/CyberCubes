#include "GLQueryPool.h"

namespace cyberCubes {
	
GLQueryPool::GLQueryPool()
{

}

GLQueryPool::~GLQueryPool()
{
	while(!freeQueryIds.empty())
	{
		GLuint id = freeQueryIds.top();
		freeQueryIds.pop();
		glDeleteQueries(1, &id);
	}
}


GLuint GLQueryPool::getQueryId()
{
	if (freeQueryIds.empty())
	{
		constexpr size_t BATCH_SIZE = 100;
		
		GLuint oqIdsAr[BATCH_SIZE];
		glGenQueries(BATCH_SIZE, oqIdsAr);
		
		for (size_t i = 0; i < BATCH_SIZE; ++i)
			freeQueryIds.push(oqIdsAr[i]);
	}
	
	GLuint result = freeQueryIds.top();
	freeQueryIds.pop();
	return result;
}

void GLQueryPool::releaseQueryId(GLuint id)
{
	freeQueryIds.push(id);
}
	
}