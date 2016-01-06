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
	
	Inventory *inventory;
	bool inventoryShow = false;
	
	Camera camera;
	
	GLuint crosshairVao = 0;
	GLuint crosshairBufferObject = 0;
	Shader crosshairShader("crosshairShader");
	
	bool renderHighlight;
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

int main()
{
	if (!initConf("config.lua"))
		return 1;

	createColorMap();

	SDL_Surface *detailTextureImage = IMG_Load("textures/detail.jpg");
	//return 0;

	FloatSmoothing lightMultiplierSmoothing(1.0f, 1.0f);

	int width = 1500;
	int height = 1000;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	SDL_Window *window = SDL_CreateWindow("My Game Window",
										  SDL_WINDOWPOS_UNDEFINED,
										  SDL_WINDOWPOS_UNDEFINED,
										  width, height,
										  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

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

	printf("Version %s", version);

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

	Shader quadShader("quadShader1");

	if (!quadShader.buildShaderProgram("quadvs1.glsl", "quadfs1.glsl"))
	{
		SDL_Quit();
		return 0;
	}

	Shader quadShader2("quadShader2");

	if (!quadShader2.buildShaderProgram("quadvs1.glsl", "quadfs2.glsl"))
	{
		SDL_Quit();
		return 0;
	}

	Shader wireframeBox("wireframeBoxShader");

	if (!wireframeBox.buildShaderProgram("wireframeBoxvs.glsl", "wireframeBoxgs.glsl", "wireframeBoxfs.glsl"))
	{
		SDL_Quit();
		return 0;
	}
	
	Shader highlightSideShader("highlightSideShader");

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

	camera.position = math::vec3(0.0f, 0.0f, 300.0f);
	camera.aspectRatio = (float) width / (float) height;
	math::Frustum frustum;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	boost::timer::cpu_timer timer;

	glEnable(GL_DEPTH_TEST);

	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);

	GLuint	detailTexture;
	glGenTextures(1, &detailTexture);
	glBindTexture(GL_TEXTURE_2D, detailTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, detailTextureImage->w, detailTextureImage->h, 0, GL_RGB, GL_UNSIGNED_BYTE, detailTextureImage->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);


	GLuint	frameBufferTexture1;
	glGenTextures(1, &frameBufferTexture1);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture1, 0);

	GLuint	frameBufferTexture2;
	glGenTextures(1, &frameBufferTexture2);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, frameBufferTexture2, 0);
	
	GLuint	frameBufferTexture3;
	glGenTextures(1, &frameBufferTexture3);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture3);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, frameBufferTexture3, 0);


	GLuint	frameBufferMaterialTexture;
	glGenTextures(1, &frameBufferMaterialTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R16I, width, height, 0, GL_RED_INTEGER, GL_SHORT, NULL);
	//glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture, 0);

	GLuint	depth_tex;
	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	GLuint	frameBufferFinalTexture;
	glGenTextures(1, &frameBufferFinalTexture);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferFinalTexture);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

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
						std::cout << "Wrong image " << type << std::endl;

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

	GLuint HSColorTexture;
	glGenTextures(1, &HSColorTexture);
	glBindTexture(GL_TEXTURE_2D, HSColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, colorMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0,
							GL_COLOR_ATTACHMENT1,
							GL_COLOR_ATTACHMENT2,
							GL_COLOR_ATTACHMENT3
						   };
	glDrawBuffers(4, drawBuffers);

	GLenum status;
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
	
	GLuint higlightSideVao = 0;
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
	
	BlockRenderer renderer;
	inventory = new Inventory(*vg, renderer);
	inventory->setWindowSize(width, height);
	
	GLuint timerQuery;
	glGenQueries(1, &timerQuery);
	
	inventory->refreshNEIItems();
	
	input::MainInput input(&world, inventory, &camera);
	
	while (input.gameLoopRunning)
	{
		soundSystem.update();
		
		glBeginQuery(GL_TIME_ELAPSED, timerQuery);
		
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

		std::vector<IntCoord> nonOpaqueChunks;
		{
			checkGLError("1");
			//boost::timer::auto_cpu_timer t;
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture1, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, frameBufferTexture2, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, frameBufferTexture3, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture, 0);

			//glDrawBuffer(GL_COLOR_ATTACHMENT0);
			//glReadBuffer(GL_COLOR_ATTACHMENT0);
			//GLenum drawBufs[] = {GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT1};

			//glDrawBuffers(1, drawBufs);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);

			checkGLError("2");
			//glEnable(GL_POLYGON_OFFSET_FILL);
			player.updatePosition(camera.position);
			world.renderPass1(camera, player, nonOpaqueChunks);
			//glDisable(GL_POLYGON_OFFSET_FILL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferFinalTexture, 0);

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
				glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture1);
			}

			if (quadShader.uniforms.count("frameBufferTexture2"))
			{
				glUniform1i(quadShader.uniforms["frameBufferTexture2"], 1);
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture2);
			}
			
			if (quadShader.uniforms.count("frameBufferTexture3"))
			{
				glUniform1i(quadShader.uniforms["frameBufferTexture3"], 2);
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture3);
			}

			if (quadShader.uniforms.count("materialSampler"))
			{
				glUniform1i(quadShader.uniforms["materialSampler"], 3);
				glActiveTexture(GL_TEXTURE0 + 3);
				glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture);
			}

			if (quadShader.uniforms.count("blockSampler"))
			{
				glUniform1i(quadShader.uniforms["blockSampler"], 4);
				glActiveTexture(GL_TEXTURE0 + 4);
				glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
			}

			checkGLError("5");

			if (quadShader.uniforms.count("HSColorSampler"))
			{
				//std::cout << "HSColorSampler " << HSColorTexture << std::endl;
				glUniform1i(quadShader.uniforms["HSColorSampler"], 5);
				glActiveTexture(GL_TEXTURE0 + 5);
				glBindTexture(GL_TEXTURE_2D, HSColorTexture);
			}

			if (quadShader.uniforms.count("detailSampler"))
			{
				//std::cout << "HSColorSampler " << HSColorTexture << std::endl;
				glUniform1i(quadShader.uniforms["detailSampler"], 6);
				glActiveTexture(GL_TEXTURE0 + 6);
				glBindTexture(GL_TEXTURE_2D, detailTexture);
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
				
				world.renderPass2(camera, player, nonOpaqueChunks, blockTexture, HSColorTexture);
				
				if (b)
				{
					if (renderHighlight)
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

					if (input.isPlayerMode())
					{
						int blockType = inventory->getHotbarCell(inventory->getSelectedHotbarSlot()).blockId;
						if (blockType > 0)
						{
							if (input.rClicked || (input.rPressed && input.keys.count(SDLK_LSHIFT)))
							{
								world.putBlock(prevBPos, blockType);

								std::cout << "ADDB " << eucModChunk(prevBPos.x) << " " << eucModChunk(prevBPos.y) << " " << eucModChunk(prevBPos.z) << std::endl;
							}
						}

						if (input.lClicked || (input.lPressed && input.keys.count(SDLK_LSHIFT)))
						{
							world.putBlock(bpos, 0);
						}
					}
				}
			}

			checkGLError("9");
			
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
				glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferFinalTexture);
			}

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glUseProgram(0);

			glBindTexture(GL_TEXTURE_2D, 0);

			glFinish();
		}
		
		glEndQuery(GL_TIME_ELAPSED);
		
		GLint available = 0;
		while (!available) {
			glGetQueryObjectiv(timerQuery, GL_QUERY_RESULT_AVAILABLE, &available);
		}
		
		GLuint64 timeElapsed;
		glGetQueryObjectui64v(timerQuery, GL_QUERY_RESULT, &timeElapsed);
		g_timeElapsed = g_timeElapsed * 0.95 + timeElapsed * 0.05;

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
	oss << "Frame time " << std::setprecision(2) << g_timeElapsed / 1.0e6 << "ms";
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
