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

struct FloatSmoothing
{
	float value, period;

	FloatSmoothing(float value, float period) : value(value), period(period) {}

	float set(float new_val, float dt)
	{
		value += (new_val - value) * (1.0 - std::exp(-dt / period));
		return value;
	}
};

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



namespace
{
	double g_timeElapsed = 0;
	
	Inventory *inventory;
	bool inventoryShow = false;
	
	Camera camera;
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

	camera.position = math::vec3(0.0f, 0.0f, 300.0f);
	camera.aspectRatio = (float) width / (float) height;
	math::Frustum frustum;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	math::vec3 vel = math::vec3(0.0f, 0.0f, 0.0f);
	std::set<int> keys, pressedKeys;

	char bGameLoopRunning = 1;
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
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture1, 0);

	GLuint	frameBufferTexture2;
	glGenTextures(1, &frameBufferTexture2);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, frameBufferTexture2, 0);
	
	GLuint	frameBufferTexture3;
	glGenTextures(1, &frameBufferTexture3);
	glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture3);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
			if (textureNameIdMap.count(block->mainTexture))
			{
				block->textureId = textureNameIdMap[block->mainTexture];
			}
			else
			{
				SDL_Surface *image = IMG_Load(block->mainTexture.c_str());

				if (!image || image->w != 16 || image->h != 16)
				{
					std::cout << "Wrong image " << type << std::endl;

					block->textureId = 0;
					textureNameIdMap[block->mainTexture] = 0;
					continue;
				}

				images[++blockTextureId] = image;
				block->textureId = blockTextureId;
				textureNameIdMap[block->mainTexture] = blockTextureId;
			}
		}
	}

	GLuint blockTexture;
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
				Uint8 *p = (Uint8 *)image->pixels + y * image->pitch + x * bpp;

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
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 4);

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

	bool pause = false;

	glPolygonOffset(1.0, 1.0);

	bool flyMode = true;

	double globalT = 0.0f;

	int blockType = 1;

	NVGcontext* vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (!vg)
	{
		std::cout << "Could not init nanovg." << std::endl;
		return -1;
	}
	
	if (!loadFonts(vg))
		return -1;
	
	inventory = new Inventory(*vg);
	
	GLuint timerQuery;
	glGenQueries(1, &timerQuery);
	
	bool lPressed = false;
	bool rPressed = false;
	
	while (bGameLoopRunning)
	{
		glBeginQuery(GL_TIME_ELAPSED, timerQuery);
		
		const boost::timer::cpu_times elapsed_times = timer.elapsed();
		boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
		float dt = (double) elapsed / 1.0e9;
		globalT += dt;

		timer.start();
		SDL_Event e;

		pressedKeys.clear();
		bool rClicked = false;
		bool lClicked = false;

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				bGameLoopRunning = 0;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
				bGameLoopRunning = 0;

			if (e.type == SDL_KEYDOWN)
			{
				keys.insert(e.key.keysym.sym);
				pressedKeys.insert(e.key.keysym.sym);
			}

			if (e.type == SDL_KEYUP)
			{
				keys.erase(e.key.keysym.sym);
			}

			if (!pause)
			{
				if (e.type == SDL_MOUSEMOTION)
				{
					camera.rotate(e.motion.xrel, e.motion.yrel);
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
			{
				rClicked = true;
				rPressed = true;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				lClicked = true;
				lPressed = true;
			}
			
			if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT)
			{
				rPressed = false;
			}
			
			if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
			{
				lPressed = false;
			}
		}

		if (pressedKeys.count(SDLK_PAUSE))
		{
			pause = !pause;
			keys.erase(SDLK_PAUSE);
			SDL_SetRelativeMouseMode(pause ? SDL_FALSE : SDL_TRUE);
		}

		if (pressedKeys.count(SDLK_f))
		{
			flyMode = !flyMode;

			std::cout << "flyMode " << flyMode << std::endl;
		}

		if (pressedKeys.count(SDLK_1))
			blockType = 8;

		if (pressedKeys.count(SDLK_2))
			blockType = 9;

		if (pressedKeys.count(SDLK_3))
			blockType = 10;
		
		if (pressedKeys.count(SDLK_4))
			blockType = 11;
		
		if (pressedKeys.count(SDLK_5))
			blockType = 30015;
		
		if (pressedKeys.count(SDLK_6))
			blockType = 30016;
		
		if (pressedKeys.count(SDLK_7))
			blockType = 30017;
		
		if (pressedKeys.count(SDLK_8))
			blockType = 30018;
		
		if (pressedKeys.count(SDLK_9))
			blockType = 30019;

		if (pressedKeys.count(SDLK_e))
			inventoryShow = !inventoryShow;
		
		
		if (flyMode)
		{
			math::vec3 accel = math::vec3(0.0f, 0.0f, 0.0f);

			if (keys.count(SDLK_w))
				accel.y += 1.0;

			if (keys.count(SDLK_s))
				accel.y -= 1.0;

			if (keys.count(SDLK_d))
				accel.x += 1.0;

			if (keys.count(SDLK_a))
				accel.x -= 1.0;

			if (keys.count(SDLK_SPACE))
				accel.z += 1.0;

			if (keys.count(SDLK_LCTRL))
				accel.z -= 1.0;

			if (!keys.count(SDLK_w) && !keys.count(SDLK_s))
			{
				int sgn = (vel.y < 0.0f) ? -1 : ((vel.y > 0.0f) ? 1 : 0);
				accel.y = -sgn;
			}

			if (!keys.count(SDLK_d) && !keys.count(SDLK_a))
			{
				int sgn = (vel.x < 0.0f) ? -1 : ((vel.x > 0.0f) ? 1 : 0);
				accel.x = -sgn;
			}

			if (!keys.count(SDLK_SPACE) && !keys.count(SDLK_LCTRL))
			{
				int sgn = (vel.z < 0.0f) ? -1 : ((vel.z > 0.0f) ? 1 : 0);
				accel.z = -sgn;
			}

			accel *= 20.0f;

			math::vec3 oldVel = vel;
			vel += accel * dt;

			if (!keys.count(SDLK_w) && !keys.count(SDLK_s) && vel.y * oldVel.y < 0.0f)
				vel.y = 0.0f;

			if (!keys.count(SDLK_d) && !keys.count(SDLK_a) && vel.x * oldVel.x < 0.0f)
				vel.x = 0.0f;

			if (!keys.count(SDLK_SPACE) && !keys.count(SDLK_LCTRL) && vel.z * oldVel.z < 0.0f)
				vel.z = 0.0f;

			float maxVel = 50.0f;

			if (math::length(vel) > maxVel)
				vel *= (maxVel / math::length(vel));

			math::vec3 dPos = vel * dt;

			math::vec3 playerSize = math::vec3(0.6f, 0.6f, 1.7f);
			math::vec3 eyeOffset = math::vec3(0.3f, 0.3f, 1.6f);
			math::BBox b(camera.position - eyeOffset, playerSize);

			dPos = camera.transform(dPos.y, dPos.x, dPos.z);
			world.move(b, dPos);
			dPos = b.point - (camera.position - eyeOffset);

			math::vec3 cdPos = camera.untransform(dPos);
			cdPos = math::vec3(cdPos.y, cdPos.x, cdPos.z);
			vel = cdPos / dt;

			camera.position += dPos;
		}
		else
		{
			math::vec3 accel = math::vec3(0.0f, 0.0f, 0.0f);
			const float MAX_ACCEL = 10.0f;

			math::vec3 dirF = camera.transform(1.0, 0.0f, 0.0f);
			dirF.z = 0.0f;
			dirF = math::normalize(dirF) * MAX_ACCEL;

			math::vec3 dirR = camera.transform(0.0, 1.0f, 0.0f);
			dirR.z = 0.0f;
			dirR = math::normalize(dirR) * MAX_ACCEL;
			
			math::vec3 playerSize = math::vec3(0.6f, 0.6f, 1.7f);
			math::vec3 widePlayerSize = math::vec3(0.6f, 0.8f, 1.7f);
			math::vec3 eyeOffset = math::vec3(0.3f, 0.3f, 1.6f);
			bool touchGround = false, wideTouchGround = false;
			{
				math::vec3 corner = camera.position - eyeOffset;
				math::BBox b(corner, playerSize);
				math::vec3 dPos =  math::vec3(0, 0, -0.2f);
				world.move(b, dPos);
			
				if (b.point.z > (corner.z + dPos.z + 1e-3))
					touchGround = true;
				
				b.point = corner;
				b.size = widePlayerSize;
				
				world.move(b, dPos);
			
				if (b.point.z > (corner.z + dPos.z + 1e-3))
					wideTouchGround = true;
			}

			float velLen = math::length(vel);
			
			if (touchGround)
			{
				if (keys.count(SDLK_w))
					accel += dirF;

				if (keys.count(SDLK_s))
					accel -= dirF;

				if (keys.count(SDLK_d))
					accel += dirR;

				if (keys.count(SDLK_a))
					accel -= dirR;

				if (pressedKeys.count(SDLK_SPACE))
					vel.z = 5.0f;
			}
			
			if (wideTouchGround && !pressedKeys.count(SDLK_SPACE))
			{
				if (math::length(accel) > 0.01)
					vel.z += 0.2 / (1.0f + velLen / 10.0f);
			}
			
			vel += accel * dt / (1.0f + velLen / 10.0f);
			vel.z -= 9.8f * dt;			
			float fCoef = math::pow(0.98f, dt);

			vel *= fCoef;
			
			if (touchGround)
			{
				math::vec3 hv = math::vec3(vel.x, vel.y, 0.0f);
				const float DV = 5.0f * dt;

				if (math::length(hv) > DV)
					hv = math::normalize(hv) * DV;

				vel -= hv;
			}

			math::vec3 dPos = vel * dt;
			
			math::BBox b(camera.position - eyeOffset, playerSize);
			world.move(b, dPos);
			dPos = b.point - (camera.position - eyeOffset);
			vel = dPos / dt;

			camera.position += dPos;
		}

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_SCISSOR_TEST);
		
		world.dayNightLightCoef = (1.0 + std::cos(globalT / 20.0))/2.0;
		//world.dayNightLightCoef = 1.0f;
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
					glDrawArrays(GL_POINTS, 0, 1);

					if (wireframeBox.uniforms.count("position"))
					{
						math::vec3 pos = (math::vec3) prevBPos;
						glUniform3fv(wireframeBox.uniforms["position"], 1, &pos[0]);
					}


					glDrawArrays(GL_POINTS, 0, 1);
					glBindVertexArray(0);

					if (rClicked || (rPressed && keys.count(SDLK_LSHIFT)))
					{
						world.putBlock(prevBPos, blockType);

						std::cout << "ADDB " << eucModChunk(prevBPos.x) << " " << eucModChunk(prevBPos.y) << " " << eucModChunk(prevBPos.z) << std::endl;
					}

					if (lClicked || (lPressed && keys.count(SDLK_LSHIFT)))
					{
						world.putBlock(bpos, 0);
					}
				}
			}

			checkGLError("9");
			
			world.renderPass2(camera, player, nonOpaqueChunks, blockTexture, HSColorTexture);

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

		if (pressedKeys.count(SDLK_F2))
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
	oss << "Pos " << std::fixed << std::setprecision(0) << camera.position.x << " " << camera.position.y << " " << camera.position.z;
	nvgText(vg, 0, 40, oss.str().c_str(), nullptr);
	
	if (inventoryShow)
		inventory->render(w, h);
	
	nvgEndFrame(vg);
}
