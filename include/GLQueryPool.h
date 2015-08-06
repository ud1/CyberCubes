#ifndef GLQUERYPOOL_H
#define GLQUERYPOOL_H

#include <GL/glew.h>
#include <stack>

namespace cyberCubes {

class GLQueryPool
{
public:
	GLQueryPool();
	~GLQueryPool();
	
	GLuint getQueryId();
	void releaseQueryId(GLuint id);
	
	std::stack<GLuint> freeQueryIds;
};

}

#endif // GLQUERYPOOL_H
