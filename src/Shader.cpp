#include "Shader.h"

#include <iostream>

static GLuint createShader(GLenum eShaderType, const char *strShaderFile)
{
	char shaderSource[4096];
	char inChar;
	FILE *shaderFile;
	int i = 0;

	shaderFile = fopen(strShaderFile, "r");
	while(fscanf(shaderFile,"%c",&inChar) > 0)
	{
		shaderSource[i++] = inChar; //loading the file's chars into array
	}
	shaderSource[i - 1] = '\0';
	fclose(shaderFile);
	puts(shaderSource); //print to make sure the string is loaded

	GLuint shader = glCreateShader(eShaderType);
	const char *ss = shaderSource;
	glShaderSource(shader, 1, &ss, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
        	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        	GLchar strInfoLog[4096];
        	glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		char strShaderType[16];
		switch(eShaderType)
		{
			case GL_VERTEX_SHADER: sprintf(strShaderType, "vertex"); break;
			case GL_GEOMETRY_SHADER: sprintf(strShaderType, "geometry"); break;
			case GL_FRAGMENT_SHADER: sprintf(strShaderType, "fragment"); break;
		}

		printf("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		return -1;
	}
	else
		puts("Shader compiled sucessfully!");

	return shader;
}

bool Shader::buildShaderProgram(const char *vsPath, const char *fsPath)
{
	GLuint vertexShader;
	GLuint fragmentShader;

	vertexShader = createShader(GL_VERTEX_SHADER, vsPath);
	fragmentShader = createShader(GL_FRAGMENT_SHADER, fsPath);

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program); //linking!

	//error checking
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
    {
		GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar strInfoLog[4096];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		printf("Shader linker failure: %s\n", strInfoLog);
		return false;
	}
	else
		puts("Shader linked sucessfully!");

	/*glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);*/

	GLint nuni;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &nuni);
	char name[256];
	for (GLint i = 0; i < nuni; ++i)
	{
		GLint size;
		GLenum type;

		glGetActiveUniform(program, i, sizeof(name), NULL, &size, &type, name);
		GLint location = glGetUniformLocation(program, name);
		uniforms[name] = location;
		std::cout << "Shader " << name << " " << location << std::endl;
	}

	return true;
}

bool Shader::buildShaderProgram(const char *vsPath, const char *gsPath, const char *fsPath)
{
	GLuint vertexShader;
	GLuint geometryShader;
	GLuint fragmentShader;

	vertexShader = createShader(GL_VERTEX_SHADER, vsPath);
	geometryShader = createShader(GL_GEOMETRY_SHADER, gsPath);
	fragmentShader = createShader(GL_FRAGMENT_SHADER, fsPath);

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, geometryShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program); //linking!

	//error checking
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
    {
		GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar strInfoLog[4096];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		printf("Shader linker failure: %s\n", strInfoLog);
		return false;
	}
	else
		puts("Shader linked sucessfully!");

	/*glDetachShader(program, vertexShader);
	glDetachShader(program, geometryShader);
	glDetachShader(program, fragmentShader);*/

	GLint nuni;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &nuni);
	char name[256];
	for (GLint i = 0; i < nuni; ++i)
	{
		GLint size;
		GLenum type;

		glGetActiveUniform(program, i, sizeof(name), NULL, &size, &type, name);
		GLint location = glGetUniformLocation(program, name);
		uniforms[name] = location;
		std::cout << "Shader " << name << " " << location << std::endl;
	}

	return true;
}
