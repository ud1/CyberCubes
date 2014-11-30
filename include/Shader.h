#ifndef SHADER_H
#define SHADER_H
#include <map>
#include <string>
#include <GL/glew.h>

struct Shader
{
	GLuint program = 0;
	std::map<std::string, GLint> uniforms;

	bool buildShaderProgram(const char *vsPath, const char *fsPath);
	bool buildShaderProgram(const char *vsPath, const char *gsPath, const char *fsPath);
};

#endif // SHADER_H
