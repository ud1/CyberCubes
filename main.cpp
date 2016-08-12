#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "Camera.hpp"
#include <set>
#include <time.h> 

#include <boost/timer/timer.hpp>
#include <cmath>
#include <boost/scoped_ptr.hpp>
#include "Chunk.hpp"
#include "RenderData.hpp"
#include "World.hpp"
#include "Frustum.hpp"
#include "Shader.hpp"
#include "colorTransform.hpp"
#include <lua.h>
#include <lualib.h>
#include <tolua.h>
#include <sstream>
#include <iomanip> 

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "Player.hpp"
#include "Block.hpp"
#include "Inventory.hpp"
#include "BlockRenderer.hpp"
#include "SoundSystem.h"
#include "Input.hpp"
#include "ModelTextures.hpp"
#include "ModelRenderer.hpp"
#include "models.hpp"

static const struct
{
	unsigned int 	 width;
	unsigned int 	 height;
	unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
	unsigned char	 pixel_data[16 * 16 * 3 + 1];
} dummy_image =
{
	16, 16, 3,
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0"
	"\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0\377\377\377\0\0\0",
};

unsigned char colorMap[256 * 256 * 3];
void createColorMap()
{
	for (int h = 0; h < 256; ++h)
	{
		for (int s = 0; s < 256; ++s)
		{
			int pos = h + s * 256;
			float H = (float) h / 255.0f;
			float S = (float) s / 255.0f;
			float R, G, B;
			hsi_norm_rgb(H, S, 1.0f / 3.0f, R, G, B);

			colorMap[pos * 3 + 0] = R * 255.0f;
			colorMap[pos * 3 + 1] = G * 255.0f;
			colorMap[pos * 3 + 2] = B * 255.0f;
		}
	}
}

static void l_message(const char *pname, const char *msg)
{
	if (pname)
		fprintf(stderr, "%s: ", pname);

	fprintf(stderr, "%s\n", msg);
	fflush(stderr);
}

static int report(lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1))
	{
		const char *msg = lua_tostring(L, -1);

		if (msg == NULL) msg = "(error object is not a string)";

		l_message(NULL, msg);
		lua_pop(L, 1);
	}

	return status;
}

static int traceback(lua_State *L)
{
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */

	lua_getfield(L, LUA_GLOBALSINDEX, "debug");

	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 1;
	}

	lua_getfield(L, -1, "traceback");

	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 2);
		return 1;
	}

	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

static int docall(lua_State *L, int narg, int clear)
{
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
	lua_remove(L, base);  /* remove traceback function */

	/* force a complete garbage collection in case of errors */
	if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);

	return status;
}

static int dostring(lua_State *L, const char *s, const char *name)
{
	int status = luaL_loadbuffer(L, s, strlen(s), name) || docall(L, 0, 1);
	return report(L, status);
}


static int dofile(lua_State *L, const char *filename)
{
	int status = luaL_loadfile(L, filename) || docall(L, 0, 1);
	return report(L, status);
}

inline bool initConf(const char *filename)
{
	int  tolua_Block_open(lua_State *);

	lua_State *L = lua_open();
	luaopen_base(L);
	tolua_Block_open(L);

	if (dofile(L, filename))
	{
		printf("Syntax Error\n");
		return false;
	}

	lua_close(L);

	setlocale(LC_ALL, "rus");
	return true;
}

void checkGLError(const char *str)
{
	GLenum error = glGetError();

	if (error)
		printf("glError: %s %d\n", str, error);
}

bool loadFonts(NVGcontext* vg)
{
	int font;
	font = nvgCreateFont(vg, "sans", "fonts/Roboto-Regular.ttf");
	if (font == -1)
	{
		std::cout << "Could not add font regular." << std::endl;
		return false;
	}
	font = nvgCreateFont(vg, "sans-bold", "fonts/Roboto-Bold.ttf");
	if (font == -1)
	{
		std::cout << "Could not add font bold." << std::endl;
		return false;
	}
	return true;
}

void render2d(SDL_Window *window, NVGcontext* vg);

using namespace cyberCubes;
using namespace cyberCubes::gui;
using namespace cyberCubes::render;

namespace
{
	double g_timeElapsed = 0;
	double g_shadowTime[2] = {0};
	
	Inventory *inventory;
	bool inventoryShow = false;
	
	Camera camera;
	
	GLuint crosshairVao = 0;
	GLuint crosshairBufferObject = 0;
	Shader crosshairShader("crosshairShader");
	Shader quadShader("quadShader1");
	Shader quadShader2("quadShader2");
	Shader wireframeBox("wireframeBoxShader");
	Shader highlightSideShader("highlightSideShader");
	
	GLuint higlightSideVao = 0;
}

GLuint blockTexture;

void renderCrosshair(SDL_Window *window)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	
	float fw = w;
	float fh = h;
	float HW = 1;
	float HH = 10;
	
	std::vector<math::vec2> coords;
	coords.push_back(math::vec2(-HW + fw/2.0f, HH + fh/2.0f));
	coords.push_back(math::vec2(-HW + fw/2.0f, -HH + fh/2.0f));
	coords.push_back(math::vec2(HW + fw/2.0f, -HH + fh/2.0f));
	
	coords.push_back(math::vec2(-HW + fw/2.0f, HH + fh/2.0f));
	coords.push_back(math::vec2(HW + fw/2.0f, -HH + fh/2.0f));
	coords.push_back(math::vec2(HW + fw/2.0f, HH + fh/2.0f));
	
	coords.push_back(math::vec2(-HH + fw/2.0f, HW + fh/2.0f));
	coords.push_back(math::vec2(-HH + fw/2.0f, -HW + fh/2.0f));
	coords.push_back(math::vec2(HH + fw/2.0f, -HW + fh/2.0f));
	
	coords.push_back(math::vec2(-HH + fw/2.0f, HW + fh/2.0f));
	coords.push_back(math::vec2(HH + fw/2.0f, -HW + fh/2.0f));
	coords.push_back(math::vec2(HH + fw/2.0f, HW + fh/2.0f));
	
	glBindBuffer(GL_ARRAY_BUFFER, crosshairBufferObject);
	glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(math::vec2), coords.data(), GL_STREAM_DRAW);
	checkGLError("10_2");
	
	glBindVertexArray(crosshairVao);
	checkGLError("10_1");
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	
	glUseProgram(crosshairShader.program);

	if (crosshairShader.uniforms.count("MVP"))
	{
		math::mat4 MVP = math::ortho(0.0f, fw, 0.0f, fh);
		
		glUniformMatrix4fv(crosshairShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
	}
	
	checkGLError("10_0");
	
	glDrawArrays(GL_TRIANGLES, 0, coords.size());
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	checkGLError("10");
}

struct Globals
{
	int width = 1500;
	int height = 1000;
	int shadowMapSize = 1024;
	
	GLuint	detailTexture;
	GLuint	frameBufferTexture1, frameBufferTexture2, frameBufferFinalTexture, HSColorTexture;
	
	model::ModelChest testModel;
	
	
} globals;

void loadDetailTexture()
{
	SDL_Surface *detailTextureImage = IMG_Load("textures/detail.jpg");
	
	glGenTextures(1, &globals.detailTexture);
	glBindTexture(GL_TEXTURE_2D, globals.detailTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, detailTextureImage->w, detailTextureImage->h, 0, GL_RGB, GL_UNSIGNED_BYTE, detailTextureImage->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	SDL_FreeSurface(detailTextureImage);
}

void createTextures()
{
	glGenTextures(1, &globals.frameBufferTexture1);
	glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferTexture1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, globals.width, globals.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &globals.frameBufferTexture2);
	glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, globals.width, globals.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenTextures(1, &globals.frameBufferFinalTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferFinalTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, globals.width, globals.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glGenTextures(1, &globals.HSColorTexture);
	glBindTexture(GL_TEXTURE_2D, globals.HSColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, colorMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

int loadShaders()
{
	if (!quadShader.buildShaderProgram("quadvs1.glsl", "quadfs1.glsl"))
	{
		SDL_Quit();
		return 0;
	}

	if (!quadShader2.buildShaderProgram("quadvs1.glsl", "quadfs2.glsl"))
	{
		SDL_Quit();
		return 0;
	}

	
	if (!wireframeBox.buildShaderProgram("wireframeBoxvs.glsl", "wireframeBoxgs.glsl", "wireframeBoxfs.glsl"))
	{
		SDL_Quit();
		return 0;
	}
	
	if (!highlightSideShader.buildShaderProgram("highlightSidevs.glsl", "highlightSidegs.glsl", "highlightSidefs.glsl"))
	{
		SDL_Quit();
		return 0;
	}
	
	if (!crosshairShader.buildShaderProgram("crosshairShadervs.glsl", "crosshairShaderfs.glsl"))
	{
		SDL_Quit();
		return 0;
	}
}

void loadTextures()
{
	int blockTextureId = 0;
	const int MAX_BLOCK_TYPES = 40000;
	std::map<int, SDL_Surface *> images;
	std::map<std::string, int> textureNameIdMap;
	
	for (int type = 0; type < MAX_BLOCK_TYPES; ++type)
	{
		Block *block = Block::get(type);

		if (block)
		{
			for (int i = 0; i < 6; ++i)
			{
				std::string textureName = block->getTexture(i);
				if (textureNameIdMap.count(textureName))
				{
					block->textureId[i] = textureNameIdMap[textureName];
				}
				else
				{
					SDL_Surface *image = IMG_Load(textureName.c_str());

					if (!image || image->w != 16 || image->h != 16)
					{
						std::cout << "Invalid image attributes " << type << std::endl;

						block->textureId[i] = 0;
						textureNameIdMap[textureName] = 0;
						continue;
					}

					images[++blockTextureId] = image;
					block->textureId[i] = blockTextureId;
					textureNameIdMap[textureName] = blockTextureId;
				}
			}
		}
	}

	glGenTextures(1, &blockTexture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, GL_RGBA8, 16, 16, blockTextureId + 1);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGB, GL_UNSIGNED_BYTE, dummy_image.pixel_data);

	for (std::map<int, SDL_Surface *>::iterator it = images.begin(); it != images.end(); ++it)
	{
		SDL_Surface *image = it->second;
		int bpp = image->format->BytesPerPixel;

		unsigned char data[16 * 16 * 4];

		for (int y = 0; y < 16; ++y)
		{
			for (int x = 0; x < 16; ++x)
			{
				Uint8 *p = (Uint8 *)image->pixels + (15 - y) * image->pitch + x * bpp;

				data[(y * 16 + x) * 4 + 0] = p[0];
				data[(y * 16 + x) * 4 + 1] = p[1];
				data[(y * 16 + x) * 4 + 2] = p[2];
				
				if (bpp == 4)
					data[(y * 16 + x) * 4 + 3] = p[3];
				else
					data[(y * 16 + x) * 4 + 3] = 255;
			}
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, it->first, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		SDL_FreeSurface(image);
		
		std::cout << "Image " << it->first << " Loaded" << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 4);
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	std::cout << "Anisotropy level " << aniso << std::endl;
}

void renderHighlight(const math::ivec3 &bpos, const math::ivec3 &prevBPos)
{
	glBindVertexArray(higlightSideVao);
						checkGLError("9_1");
						glUseProgram(highlightSideShader.program);
						glEnable(GL_DEPTH_TEST);
						if (highlightSideShader.uniforms.count("MVP"))
						{
							math::mat4 MVP = camera.getVP();
							glUniformMatrix4fv(highlightSideShader.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
						}
						
						if (highlightSideShader.uniforms.count("position"))
						{
							math::vec3 pos = (math::vec3) prevBPos;
							glUniform3fv(highlightSideShader.uniforms["position"], 1, &pos[0]);
						}
						
						int normalIndex = 0;
						if (bpos.x > prevBPos.x)
							normalIndex = (int) Dir::XP;
						else if (bpos.x < prevBPos.x)
							normalIndex = (int) Dir::XN;
						else if (bpos.y > prevBPos.y)
							normalIndex = (int) Dir::YP;
						else if (bpos.y < prevBPos.y)
							normalIndex = (int) Dir::YN;
						else if (bpos.z > prevBPos.z)
							normalIndex = (int) Dir::ZP;
						else if (bpos.z < prevBPos.z)
							normalIndex = (int) Dir::ZN;
						
						if (highlightSideShader.uniforms.count("normalIndex"))
						{
							glUniform1i(highlightSideShader.uniforms["normalIndex"], normalIndex);
						}
						
						if (highlightSideShader.uniforms.count("blockSampler"))
						{
							glUniform1i(highlightSideShader.uniforms["blockSampler"], 4);
							glActiveTexture(GL_TEXTURE0 + 0);
							glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
						}

						glPolygonOffset(-1.0, -1.0);
						glEnable(GL_POLYGON_OFFSET_FILL);
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						
						checkGLError("9_2");
						
						glDrawArrays(GL_POINTS, 0, 1);
						glDisable(GL_POLYGON_OFFSET_FILL);
						glBindVertexArray(0);
}

int shadow_mode = 3;

int main()
{
	if (!initConf("config.lua"))
		return 1;

	createColorMap();

	
	//return 0;

	FloatSmoothing lightMultiplierSmoothing(1.0f, 1.0f);

	

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	SDL_Window *window = SDL_CreateWindow("My Game Window",
										  SDL_WINDOWPOS_UNDEFINED,
										  SDL_WINDOWPOS_UNDEFINED,
										  globals.width, globals.height,
										  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if (glContext == NULL)
	{
		printf("There was an error creating the OpenGL context!\n");
		return 0;
	}

	const unsigned char *version = glGetString(GL_VERSION);

	if (version == NULL)
	{
		printf("There was an error creating the OpenGL context!\n");
		return 1;
	}

	printf("Version %s\n", version);
	
	int textureUnits;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &textureUnits);
	printf("Max texture units %d\n", textureUnits);

	SDL_GL_MakeCurrent(window, glContext);


	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);

	//MUST make a context AND make it current BEFORE glewInit()!
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();

	if (glew_status != 0)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	World world;
	world.create();
	
	SoundSystem soundSystem;
	soundSystem.initialize();
	//soundSystem.playBackground("music/Andrew_Seistrup_-_A_Glimpse_of_Who_She_Was.ogg", 1.0);

	Player player(&world);

	loadShaders();

	camera.position = math::vec3(0.0f, 0.0f, 300.0f);
	camera.aspectRatio = (float) globals.width / (float) globals.height;
	math::Frustum frustum;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	boost::timer::cpu_timer timer;

	glEnable(GL_DEPTH_TEST);

	
	loadDetailTexture();
	createTextures();

	


	

	GLuint	depth_tex;
	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, globals.width, globals.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	

	
	
	
	
	
	// shadow map
	GLuint shadow_map_texture1;
	glGenTextures(1, &shadow_map_texture1);
	glBindTexture(GL_TEXTURE_2D, shadow_map_texture1);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, globals.shadowMapSize, globals.shadowMapSize, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	GLfloat clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	GLuint shadow_map_texture2;
	glGenTextures(1, &shadow_map_texture2);
	glBindTexture(GL_TEXTURE_2D, shadow_map_texture2);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, globals.shadowMapSize, globals.shadowMapSize, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	GLuint box_positions_texture1;
	glGenTextures(1, &box_positions_texture1);
	glBindTexture(GL_TEXTURE_2D, box_positions_texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, globals.shadowMapSize, globals.shadowMapSize, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	GLuint box_positions_texture2;
	glGenTextures(1, &box_positions_texture2);
	glBindTexture(GL_TEXTURE_2D, box_positions_texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, globals.shadowMapSize, globals.shadowMapSize, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	GLuint shadow_map_framebuffer = 0;
	glGenFramebuffers(1, &shadow_map_framebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture2, 0);
	GLenum status;
	/*status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "FBO shadow good" << std::endl;
		break;

	default:
		std::cout << "FBO shadow error " << status << std::endl;
		return 0;
	}*/
	
	globals.testModel.position = math::vec3(10.0, 0.0, 257.0f);
	globals.testModel.setLidAngle(1.5);

	loadTextures();
	
	


	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
	glViewport(0, 0, globals.width, globals.height);

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0,
							GL_COLOR_ATTACHMENT1,
							GL_COLOR_ATTACHMENT2,
							GL_COLOR_ATTACHMENT3
						   };
	glDrawBuffers(4, drawBuffers);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "FBO good" << std::endl;
		break;

	default:
		std::cout << "FBO error " << status << std::endl;
		return 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quad[] =
	{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};

	GLuint quadVao = 0;
	GLuint quadBufferObject = 0;
	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);
	glGenBuffers(1, &quadBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, quadBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW); //formatting the data for the buffer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
	glBindVertexArray(0);

	GLuint wireframeCubeVao = 0;
	glGenVertexArrays(1, &wireframeCubeVao);
	
	
	glGenVertexArrays(1, &higlightSideVao);
	
	glGenVertexArrays(1, &crosshairVao);
	glGenBuffers(1, &crosshairBufferObject);

	glBindVertexArray(crosshairVao);
	glBindBuffer(GL_ARRAY_BUFFER, crosshairBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(math::vec2), 0);
	glBindVertexArray(0);

	glPolygonOffset(1.0, 1.0);

	double globalT = 0.0f;

	NVGcontext* vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (!vg)
	{
		std::cout << "Could not init nanovg." << std::endl;
		return -1;
	}
	
	if (!loadFonts(vg))
		return -1;
	
	////////////////////////////////
	
	
	/*math::ivec2 modelTexturesSize = math::ivec2(256, 256);
	model::ModelTextures modelTextures(modelTexturesSize);
	model::ModelRendererBuffer modelRendererBuffer;
	model::ModelRenderer modelRenderer(modelTextures);
	
	modelTextures.addTexture("textures/entity/wolf/wolf.png", "wolf");
	modelTextures.addTexture("textures/entity/chest/normal.png", "chest");
	modelTextures.upload();
	
	math::vec2 invTextureSize = modelRenderer.getInvTextureSize();
	
	globals.testModel.updateTextures(modelTextures);*/
	
	////////////////////////////////
	
	BlockRenderer renderer(world.modelRenderer);
	inventory = new Inventory(*vg, renderer);
	inventory->setWindowSize(globals.width, globals.height);
	
	GLuint timerQuery[3];
	glGenQueries(3, &timerQuery[0]);
	
	inventory->refreshNEIItems();
	
	input::MainInput input(&world, inventory, &camera);
	
	Shader simpleShader("simpleShader");
	simpleShader.buildShaderProgram("simplevs.glsl", "simplefs.glsl");
	
	// The quad's FBO. Used only for visualizing the shadowmap.
	static const GLfloat g_quad_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};
	
	GLuint simpleVao = 0;
	
	GLuint quad_vertexbuffer = 0;
	glGenVertexArrays(1, &simpleVao);
	glBindVertexArray(simpleVao);
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
	glBindVertexArray(0);
	


	
	
	////////////////////////////////
	
	while (input.gameLoopRunning)
	{
		glViewport(0, 0, globals.width, globals.height);
		
		soundSystem.update();
		
		//glBeginQuery(GL_TIME_ELAPSED, timerQuery);
		
		const boost::timer::cpu_times elapsed_times = timer.elapsed();
		boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
		float dt = (double) elapsed / 1.0e9;
		globalT += dt;

		timer.start();

		input.processInput(dt);
		inventoryShow = input.isInventoryMode();

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_SCISSOR_TEST);
		
		//world.dayNightLightCoef = (1.0 + std::cos(globalT / 20.0))/2.0;
		world.dayNightLightCoef = 1.0f;
		//std::cout << "dayNightLightCoef " << world.dayNightLightCoef << std::endl;
		
		float sunDirCoef = 0.1;
		world.sunDir = math::vec3(cos(sunDirCoef*globalT/10.0f), -sin(sunDirCoef*globalT/10.0f), 1.0f);
		//world.sunDir = math::vec3(1.0f, 1.0f, 1.0f);
		//world.sunDir = math::vec3(0.1f, 1.0f, 1.0f);
		
		world.sunDir = math::normalize(world.sunDir);

		std::vector<IntCoord> nonOpaqueChunks;
		
		float scale_shadow1 = 20.0f;
		float scale_shadow2 = 80.0f;
		
		if (shadow_mode != 4)
		{ // Shadow map
			glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer);
			glPolygonOffset(1, 1.0);
			glEnable(GL_POLYGON_OFFSET_FILL);
			
			
			
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture1, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, box_positions_texture1, 0);
			glViewport(0, 0, globals.shadowMapSize, globals.shadowMapSize);
			
			/*glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture1, 0);

			glDrawBuffer(GL_NONE); // No color buffer is drawn to.*/

			// Always check that our framebuffer is ok
			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				return -2;
			
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glBeginQuery(GL_TIME_ELAPSED, timerQuery[0]);
			world.renderShadowMap(camera, player, scale_shadow1);
			
			glEndQuery(GL_TIME_ELAPSED);
			//glDrawBuffer(GL_NONE); // No color buffer is drawn to.

			
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map_texture2, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, box_positions_texture2, 0);
			
			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				return -2;
			
			
			glBeginQuery(GL_TIME_ELAPSED, timerQuery[1]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			world.renderShadowMap(camera, player, scale_shadow2);
			glDisable(GL_POLYGON_OFFSET_FILL);
			
			glEndQuery(GL_TIME_ELAPSED);
			
			glViewport(0, 0, globals.width, globals.height);
		}
		
		glBeginQuery(GL_TIME_ELAPSED, timerQuery[2]);
		
		{	
			checkGLError("1");
			//boost::timer::auto_cpu_timer t;
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, globals.frameBufferTexture1, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, globals.frameBufferTexture2, 0);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);

			checkGLError("2");
			//glEnable(GL_POLYGON_OFFSET_FILL);
			player.updatePosition(camera.position);
			world.renderPass1(camera, player, nonOpaqueChunks, blockTexture, globals.detailTexture, shadow_map_texture1, shadow_map_texture2, box_positions_texture1, box_positions_texture2, scale_shadow1, scale_shadow2,
				globals.HSColorTexture
			);
			//glDisable(GL_POLYGON_OFFSET_FILL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, globals.frameBufferFinalTexture, 0);

			//glFlush();
			world.lightMultiplier = 1.0 / std::pow(0.02f, 1.0f - world.getMaxLightNearPoint(camera.position));

			if (world.lightMultiplier > 2.0)
				world.lightMultiplier = 2.0;

			if (world.lightMultiplier < 1.0)
				world.lightMultiplier = 1.0;

			world.lightMultiplier = lightMultiplierSmoothing.set(world.lightMultiplier, dt);

			float skyLightMultiplier = std::pow(0.05f, 1.0f - world.dayNightLightCoef);
			world.fogColor = math::vec3(0.5, 0.7, 1.0) * world.lightMultiplier * skyLightMultiplier;
			glClearColor(world.fogColor.x, world.fogColor.y, world.fogColor.z, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glUseProgram(quadShader.program);
			glBindVertexArray(quadVao);

			checkGLError("3");

			if (quadShader.uniforms.count("frameBufferTexture1"))
			{
				glUniform1i(quadShader.uniforms["frameBufferTexture1"], 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferTexture1);
			}

			if (quadShader.uniforms.count("frameBufferTexture2"))
			{
				glUniform1i(quadShader.uniforms["frameBufferTexture2"], 1);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferTexture2);
			}

			if (quadShader.uniforms.count("blockSampler"))
			{
				glUniform1i(quadShader.uniforms["blockSampler"], 2);
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
			}

			checkGLError("5");

			if (quadShader.uniforms.count(""))
			{
				//std::cout << "HSColorSampler " << HSColorTexture << std::endl;
				glUniform1i(quadShader.uniforms["HSColorSampler"], 3);
				glActiveTexture(GL_TEXTURE0 + 3);
				glBindTexture(GL_TEXTURE_2D, globals.HSColorTexture);
			}

			if (quadShader.uniforms.count("detailSampler"))
			{
				//std::cout << "HSColorSampler " << HSColorTexture << std::endl;
				glUniform1i(quadShader.uniforms["detailSampler"], 4);
				glActiveTexture(GL_TEXTURE0 + 4);
				glBindTexture(GL_TEXTURE_2D, globals.detailTexture);
			}

			checkGLError("6");

			if (quadShader.uniforms.count("lightMultiplier"))
			{
				//std::cout << "lightMultiplier " << lightMultiplier << std::endl;
				glUniform1f(quadShader.uniforms["lightMultiplier"], world.lightMultiplier);
			}
			
			if (quadShader.uniforms.count("fogColor"))
			{
				glUniform3fv(quadShader.uniforms["fogColor"], 1, &world.fogColor[0]);
			}

			checkGLError("7");


			glDrawArrays(GL_TRIANGLES, 0, 6);
			checkGLError("7_1");
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_1D, 0);

			checkGLError("8");
			
			

			{
				math::vec3 dir = camera.transform(1.0f, 0.0f, 0.0f);
				math::ivec3 bpos, prevBPos;
				bool b = world.getBlock(camera.position, dir, 10.0f, bpos, prevBPos);

				if (b)
				{

					//std::cout << "POS " << prevBPos.x << " " << prevBPos.y << " " << prevBPos.z << std::endl;
					/*LightValue *lv = world.getLightRef<LIGHT_SUN>(prevBPos);
					LightValue *lv2 = world.getLightRef<LIGHT_SUN>(bpos);
					if (lv)
						std::cout << "POSLV " << prevBPos.x << " " << prevBPos.y << " " << prevBPos.z << " " << (int) *lv << std::endl;
					if (lv2)
						std::cout << "POSLV2 " << bpos.x << " " << bpos.y << " " << bpos.z << " " << (int) *lv2 << std::endl;/**/

					glUseProgram(wireframeBox.program);
					glEnable(GL_DEPTH_TEST);
					glBindVertexArray(wireframeCubeVao);

					if (wireframeBox.uniforms.count("MVP"))
					{
						math::mat4 MVP = camera.getVP();
						glUniformMatrix4fv(wireframeBox.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
					}

					if (wireframeBox.uniforms.count("position"))
					{
						math::vec3 pos = (math::vec3) bpos;
						glUniform3fv(wireframeBox.uniforms["position"], 1, &pos[0]);
					}

					glLineWidth(2.0f);
					glDepthMask(GL_TRUE);
					glDrawArrays(GL_POINTS, 0, 1);
					glDepthMask(GL_FALSE);
				}
				
				world.renderPass2(camera, player, nonOpaqueChunks, blockTexture, globals.HSColorTexture);
				
				if (b)
				{

					//renderHighlight(bpos, prevBPos);
					

					if (input.isPlayerMode())
					{
						int blockType = inventory->getHotbarCell(inventory->getSelectedHotbarSlot()).blockId;
						if (blockType > 0)
						{
							if (input.rClicked || (input.rPressed && input.keys.count(SDLK_LSHIFT)))
							{
								Block * block = Block::get(blockType);
								math::vec3 upDir = camera.transform(0.0f, 0.0f, 1.0f);
								int rotation = getRotation(dir, upDir, block->rotationGroup);
								world.putBlock(prevBPos, blockType, rotation);

								std::cout << "ADDB " << eucModChunk(prevBPos.x) << " " << eucModChunk(prevBPos.y) << " " << eucModChunk(prevBPos.z) << std::endl;
							}
						}

						if (input.lClicked || (input.lPressed && input.keys.count(SDLK_LSHIFT)))
						{
							world.putBlock(bpos, 0, 0);
						}
					}
				}
			}

			checkGLError("9");
			
			
			///////////////////////////
			/*std::vector<model::Vertex> modelBuffer;
			static float rt = 0.0f;
			rt += 0.001f;
			//globals.testModel.boxes[0].rotationFw = rt;
			globals.testModel.rotation = 0;
			globals.testModel.render(modelBuffer, invTextureSize);
			modelRendererBuffer.upload(modelBuffer);
			modelRenderer.startRender();
			modelRenderer.render(modelRendererBuffer, camera);
			modelRenderer.finishRender();*/
			
			
			//////////////////////////
			
			renderCrosshair(window);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDrawBuffer(GL_BACK);
			glReadBuffer(GL_BACK);

			//glClear(GL_DEPTH_BUFFER_BIT);

			glUseProgram(quadShader2.program);
			glBindVertexArray(quadVao);

			if (quadShader2.uniforms.count("textureSampler"))
			{
				glUniform1i(quadShader2.uniforms["textureSampler"], 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_RECTANGLE, globals.frameBufferFinalTexture);
			}

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glUseProgram(0);

			glBindTexture(GL_TEXTURE_2D, 0);

			glFinish();
		}
		
		{// render shadow texture
			/*glViewport(0,0,512,512);
			glUseProgram(simpleShader.program);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadow_map_texture2);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			if (simpleShader.uniforms.count("shadow_map_texture"))
				glUniform1i(simpleShader.uniforms["shadow_map_texture"], 0);
			// 1rst attribute buffer : vertices
			glBindVertexArray(simpleVao);
			
			glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
			glUseProgram(0);
			glViewport(0, 0, globals.width, globals.height);*/
		}
		
		glEndQuery(GL_TIME_ELAPSED);
		
		GLint available = 0;
		while (!available) {
			glGetQueryObjectiv(timerQuery[0], GL_QUERY_RESULT_AVAILABLE, &available);
			glGetQueryObjectiv(timerQuery[1], GL_QUERY_RESULT_AVAILABLE, &available);
			glGetQueryObjectiv(timerQuery[2], GL_QUERY_RESULT_AVAILABLE, &available);
		}
		
		GLuint64 timeElapsed[3];
		glGetQueryObjectui64v(timerQuery[0], GL_QUERY_RESULT, &timeElapsed[0]);
		glGetQueryObjectui64v(timerQuery[1], GL_QUERY_RESULT, &timeElapsed[1]);
		glGetQueryObjectui64v(timerQuery[2], GL_QUERY_RESULT, &timeElapsed[2]);
		g_timeElapsed = g_timeElapsed * 0.95 + timeElapsed[2] * 0.05;
		g_shadowTime[0] = g_shadowTime[0] * 0.95 + timeElapsed[0] * 0.05;
		g_shadowTime[1] = g_shadowTime[1] * 0.95 + timeElapsed[1] * 0.05;

		render2d(window, vg);
		
		SDL_GL_SwapWindow(window);
		//SDL_Delay(20);

		if (input.pressedKeys.count(SDLK_F2))
		{
			int w, h;
			SDL_GetWindowSize(window, &w, &h);
			SDL_Surface *img = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
			glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
			
			int pitch = img->pitch;
			for (int y = 0; y < h/2; ++y)
			{
				Uint8 *first = (Uint8*) img->pixels + pitch * y;
				Uint8 *last =  (Uint8*) img->pixels + pitch * (h - 1 - y);
				for (int x = 0; x < pitch; ++x)
				{
					std::swap(*(first + x), *(last + x));
				}
			}
			
			std::ostringstream fileName;
			fileName << "screenshots/" << ::time(nullptr) << ".png";
			IMG_SavePNG(img, fileName.str().c_str());
			SDL_FreeSurface(img);
		}
		
		if (input.pressedKeys.count((SDLK_F5)))
		{
			shadow_mode = 0;
		}
		if (input.pressedKeys.count((SDLK_F6)))
			shadow_mode = 1;
		if (input.pressedKeys.count((SDLK_F7)))
			shadow_mode = 2;
		if (input.pressedKeys.count((SDLK_F8)))
			shadow_mode = 3;
		if (input.pressedKeys.count((SDLK_F9)))
			shadow_mode = 4;
	}

	
	nvgDeleteGL3(vg);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
	return 0;
}

int g_trisRendered = 0;

void render2d(SDL_Window *window, NVGcontext* vg)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	nvgBeginFrame(vg, w, h, 1);
	
	nvgMoveTo(vg, 200, 200);
	
	nvgFontFace(vg, "sans");
	nvgFontSize(vg, 20.0f);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFillColor(vg, nvgRGBA(50, 255, 50, 255));
	
	std::ostringstream oss;
	oss << "Frame time " << std::setprecision(2) << g_timeElapsed / 1.0e6 << "ms Shadow " << g_shadowTime[0] / 1.0e6 << " | " << g_shadowTime[1] / 1.0e6;
	nvgText(vg, 0, 0, oss.str().c_str(), nullptr);
	
	oss.str("");
	oss << "Tris rendered " << g_trisRendered / 1000 << "K";
	nvgText(vg, 0, 20, oss.str().c_str(), nullptr);
	
	oss.str("");
	oss << "Pos " << std::fixed << std::setprecision(1) << camera.position.x << " " << camera.position.y << " " << camera.position.z;
	nvgText(vg, 0, 40, oss.str().c_str(), nullptr);
	
	inventory->setWindowSize(w, h);
	if (inventoryShow)
	{
		inventory->render();
		inventory->renderNEI();
	}
	
	inventory->renderHotbar();
	
	nvgEndFrame(vg);
	
	inventory->renderHotbarItems();
	if (inventoryShow)
	{
		inventory->renderInventoryItems();
		inventory->renderNEIItems();
		inventory->renderDraggedItem();
	}
	
	checkGLError("R2");
}
