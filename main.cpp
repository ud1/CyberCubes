//compile with this command on my Ubuntu 12.04 machine:
//gcc sdl2-opengl-sample.c -o sdl2-opengl-sample -Wall -std=c99 -I/usr/local/include/SDL2 -lSDL2 -I/usr/include/GL -lGL -lGLEW -Wall
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>
#include "Camera.h"
#include <set>

#include <boost/timer/timer.hpp>
#include <cmath>
#include <boost/scoped_ptr.hpp>
#include "Chunk.h"
#include "Atlas.h"
#include "RenderData.h"
#include "World.h"
#include "Frustum.h"
#include "Shader.h"
#include "colorTransform.h"
#include <lua.h>
#include <lualib.h>
#include <tolua.h>

static const struct
{
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char	 pixel_data[16 * 16 * 3 + 1];
} block_image =
{
    16, 16, 3,
    "\177\177\177tttttttttttt\177\177\177\217\217\217\217\217\217hhh\217\217\217"
    "\177\177\177tttttttttttt\177\177\177\177\177\177\177\177\177\217\217\217"
    "\217\217\217\217\217\217\217\217\217ttttttttt\217\217\217\217\217\217\217"
    "\217\217hhh\177\177\177\177\177\177ttt\177\177\177hhhtttttthhhttttttttt\217"
    "\217\217ttt\177\177\177\177\177\177\177\177\177\217\217\217\177\177\177\177"
    "\177\177\177\177\177\217\217\217\177\177\177\177\177\177\177\177\177hhh\177"
    "\177\177\217\217\217\177\177\177\177\177\177ttttttttttttttt\177\177\177\177"
    "\177\177\217\217\217\217\217\217ttttttttttttttttttttt\217\217\217\217\217"
    "\217\217\217\217\217\217\217\217\217\217hhhtttttt\177\177\177\177\177\177"
    "\177\177\177ttttttttt\177\177\177hhh\177\177\177\177\177\177tttttthhh\177"
    "\177\177\217\217\217\217\217\217\177\177\177\200\200\200\177\177\177ttt\217"
    "\217\217\217\217\217\177\177\177\177\177\177\177\177\177\177\177\177\217"
    "\217\217\217\217\217\217\217\217hhh\177\177\177\177\177\177\177\177\177t"
    "ttttttttttttttttthhhtttttttttttt\177\177\177\177\177\177\177\177\177\217"
    "\217\217\217\217\217hhh\217\217\217ttt\177\177\177\177\177\177\177\177\177"
    "\217\217\217\217\217\217\217\217\217\217\217\217\217\217\217\217\217\217"
    "\217\217\217\177\177\177tttttttttttttttttt\177\177\177hhh\177\177\177\177"
    "\177\177ttttttttt\217\217\217\177\177\177\177\177\177\177\177\177\217\217"
    "\217tttttt\217\217\217\217\217\217ttthhhttt\217\217\217\217\217\217\217\217"
    "\217\177\177\177\177\177\177\177\177\177hhh\177\177\177\177\177\177\177\177"
    "\177ttt\177\177\177\177\177\177\177\177\177\217\217\217\217\217\217\177\177"
    "\177\177\177\177\177\177\177\217\217\217\217\217\217hhh\177\177\177ttttt"
    "tttttttttttttttt\217\217\217\217\217\217ttttttttttttttt\177\177\177\177\177"
    "\177\217\217\217\217\217\217\217\217\217\217\217\217\217\217\217\177\177"
    "\177ttttttttthhhttt\177\177\177\177\177\177\217\217\217\217\217\217\177\177"
    "\177\177\177\177\177\177\177ttttttttttttttt\177\177\177\177\177\177\217\217"
    "\217\217\217\217\177\177\177tttttt\177\177\177tttttttttttthhh\217\217\217"
    "\217\217\217\177\177\177\177\177\177\177\177\177\177\177\177ttttttttt\177"
    "\177\177\177\177\177",
};

static const struct
{
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char	 pixel_data[16 * 16 * 3 + 1];
} block_image2 =
{
    16, 16, 3,
    "\271\205\\\226lJ\226lJyU:yU:\271\205\\\226lJ\226lJyU:yU:Y=)yU:yU:\271\205"
    "\\yU:\271\205\\yU:\226lJY=)yU:yU:\226lJ\207\207\207Y=)yU:\271\205\\\226l"
    "JyU:\271\205\\\226lJY=)Y=)\271\205\\yU:yU:Y=)\271\205\\yU:yU:yU:\271\205"
    "\\yU:yU:yU:Y=)Y=)\271\205\\yU:\226lJlll\271\205\\yU:\226lJY=)yU:\271\205"
    "\\\226lJ\226lJyU:\226lJyU:\271\205\\\226lJyU:\226lJyU:\226lJ\271\205\\Y="
    ")\226lJyU:yU:\226lJY=)yU:lllyU:\226lJY=)yU:yU:Y=)\226lJ\226lJyU:\226lJY="
    ")Y=)Y=)yU:yU:Y=)yU:yU:yU:\271\205\\\271\205\\yU:yU:yU:\207\207\207yU:yU:"
    "\271\205\\\271\205\\yU:\271\205\\\271\205\\yU:\226lJyU:\226lJyU:yU:\271\205"
    "\\\271\205\\\226lJ\226lJyU:yU:\226lJY=)\226lJ\226lJyU:yU:\226lJ\226lJ\226"
    "lJyU:yU:\226lJyU:\226lJyU:Y=)yU:\226lJ\226lJyU:yU:yU:Y=)yU:yU:\226lJY=)y"
    "U:yU:Y=)Y=)yU:yU:yU:yU:yU:\271\205\\\271\205\\yU:\226lJyU:\226lJyU:\271\205"
    "\\\271\205\\yU:\271\205\\\226lJY=)\271\205\\\271\205\\Y=)\226lJ\226lJ\207"
    "\207\207yU:\226lJyU:yU:\226lJ\226lJ\271\205\\yU:\226lJlll\226lJ\226lJyU:"
    "Y=)\226lJyU:Y=)yU:Y=)\226lJyU:\226lJ\226lJ\271\205\\yU:yU:yU:yU:yU:yU:yU"
    ":\271\205\\\271\205\\yU:\226lJyU:yU:tXDyU:\226lJ\226lJyU:Y=)\271\205\\Y="
    ")yU:\271\205\\\226lJ\226lJ\226lJyU:Y=)\271\205\\yU:Y=)yU:Y=)\271\205\\\271"
    "\205\\yU:\226lJyU:yU:\226lJ\226lJ\226lJyU:\271\205\\\226lJ\226lJyU:\207\207"
    "\207yU:\226lJ\226lJyU:yU:\226lJ\226lJyU:Y=)",
};

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[16 * 16 * 3 + 1];
} light_image = {
  16, 16, 3,
  "\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356"
  "\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356"
  "\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\356\356\356\356\356\356\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\356\356\356"
  "\356\356\356\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\356\356\356\356\356\356\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\356\356\356\356\356\356\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\356\356\356"
  "\356\356\356\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\356\356\356\356\356\356\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\356\356\356\356\356\356\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\356\356\356"
  "\356\356\356\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\356\356\356\356\356\356\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\356\356\356\356\356\356\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\356\356\356"
  "\356\356\356\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\356\356\356\356\356\356\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\356\356\356\356\356\356\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\356\356\356"
  "\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356"
  "\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356\356"
  "\356\356\356\356\356\356\356\356\356\356\356\356",
};

unsigned char colorMap[256*256*3];
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
			hsi_norm_rgb(H, S, 1.0f/3.0f, R, G, B);

			colorMap[pos*3 + 0] = R * 255.0f;
			colorMap[pos*3 + 1] = G * 255.0f;
			colorMap[pos*3 + 2] = B * 255.0f;
		}
    }
}

static void l_message (const char *pname, const char *msg)
{
    if (pname)
        fprintf(stderr, "%s: ", pname);
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
}

static int report (lua_State *L, int status)
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

static int traceback (lua_State *L)
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

static int docall (lua_State *L, int narg, int clear)
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

static int dostring (lua_State *L, const char *s, const char *name)
{
    int status = luaL_loadbuffer(L, s, strlen(s), name) || docall(L, 0, 1);
    return report(L, status);
}


static int dofile (lua_State *L, const char *filename)
{
    int status = luaL_loadfile(L, filename) || docall(L, 0, 1);
    return report(L, status);
}

inline bool initConf(const char *filename)
{
    int  tolua_Block_open (lua_State*);

    lua_State* L = lua_open();
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
		value += (new_val - value)*(1.0 - std::exp(-dt/period));
		return value;
	}
};

int main()
{
    if (!initConf("config.lua"))
        return 1;

	createColorMap();
	//return 0;

	FloatSmoothing lightMultiplierSmoothing(1.0f, 1.0f);

    Atlas atlas;
    atlas.make(2);
    atlas.put(0, block_image.pixel_data);
    atlas.put(1, block_image2.pixel_data);

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

    Shader quadShader;
    if (!quadShader.buildShaderProgram("quadvs1.glsl", "quadfs1.glsl"))
    {
        SDL_Quit();
        return 0;
    }

    Shader quadShader2;
    if (!quadShader2.buildShaderProgram("quadvs1.glsl", "quadfs2.glsl"))
    {
        SDL_Quit();
        return 0;
    }

    Shader wireframeBox;
    if (!wireframeBox.buildShaderProgram("wireframeBoxvs.glsl", "wireframeBoxgs.glsl", "wireframeBoxfs.glsl"))
    {
        SDL_Quit();
        return 0;
    }

    Camera camera;
    camera.position = math::vec3(0.0f, -5.0f, 0.0f);
    camera.aspectRatio = (float) width / (float) height;
    math::Frustum frustum;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    math::vec3 vel = math::vec3(0.0f, 0.0f, 0.0f);
    std::set<int> keys, pressedKeys;

    char bGameLoopRunning = 1;
    boost::timer::cpu_timer timer;

    glEnable(GL_DEPTH_TEST);



    GLuint frameBuffer;
    glGenFramebuffers( 1, &frameBuffer);

    glBindFramebuffer ( GL_FRAMEBUFFER, frameBuffer );
    glViewport( 0, 0, width, height);

    GLuint	frameBufferTexture;
    glGenTextures   ( 1, &frameBufferTexture );
    glBindTexture   ( GL_TEXTURE_RECTANGLE, frameBufferTexture );
    glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture, 0);

    GLuint	frameBufferLightTexture;
    glGenTextures   ( 1, &frameBufferLightTexture );
    glBindTexture   ( GL_TEXTURE_RECTANGLE, frameBufferLightTexture );
    glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, frameBufferLightTexture, 0);


    GLuint	frameBufferMaterialTexture;
    glGenTextures   ( 1, &frameBufferMaterialTexture );
    glBindTexture   ( GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture );
    glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_R16I, width, height, 0, GL_RED_INTEGER, GL_SHORT, NULL );
    //glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    //glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture, 0);

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

    GLuint	frameBufferTexture2;
    glGenTextures   ( 1, &frameBufferTexture2 );
    glBindTexture   ( GL_TEXTURE_RECTANGLE, frameBufferTexture2 );
    glTexImage2D    ( GL_TEXTURE_RECTANGLE, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    //glTexParameteri ( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    GLuint	blockTexture;
    glGenTextures   ( 1, &blockTexture );
    glBindTexture   ( GL_TEXTURE_2D_ARRAY, blockTexture );
    glTexStorage3D  ( GL_TEXTURE_2D_ARRAY, 4, GL_RGB8, 16, 16, 3);
    glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGB, GL_UNSIGNED_BYTE, block_image.pixel_data);
    glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, 16, 16, 1, GL_RGB, GL_UNSIGNED_BYTE, block_image2.pixel_data);
    glTexSubImage3D ( GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, 16, 16, 1, GL_RGB, GL_UNSIGNED_BYTE, light_image.pixel_data);
    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LOD, 4);

    GLuint HSColorTexture;
    glGenTextures   ( 1, &HSColorTexture );
    glBindTexture   ( GL_TEXTURE_2D, HSColorTexture );
	glTexImage2D    ( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, colorMap);
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );


    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0,
                            GL_COLOR_ATTACHMENT1,
                            GL_COLOR_ATTACHMENT2
                           };
    glDrawBuffers(3,drawBuffers);

    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "FBO good" << std::endl;
        break;
    default:
        std::cout << "FBO error " << status << std::endl;
        return 0;
    }
    glBindFramebuffer ( GL_FRAMEBUFFER, 0);

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
    GLenum error;

    float lightMultiplier = 1.0;
    glPolygonOffset(1.0, 1.0);

    bool flyMode = true;

	double globalT = 0.0f;
    while (bGameLoopRunning)
    {
        const boost::timer::cpu_times elapsed_times = timer.elapsed();
        boost::timer::nanosecond_type const elapsed(elapsed_times.wall);
        float dt = (double) elapsed / 1.0e9;
        globalT += dt;

        timer.start();
        SDL_Event e;

        pressedKeys.clear();
        bool rClicked = false;
        bool lClicked = false;
        while ( SDL_PollEvent(&e) )
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
			}

			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				lClicked = true;
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

            if (keys.count(SDLK_w))
                accel += dirF;
            if (keys.count(SDLK_s))
                accel -= dirF;
            if (keys.count(SDLK_d))
                accel += dirR;
            if (keys.count(SDLK_a))
                accel -= dirR;
            if (pressedKeys.count(SDLK_SPACE))
            {
                vel.z = 5.0f;
            }

            vel += accel * dt;
            vel.z -= 9.8f*dt;
            float hCoef = math::pow(0.5f, dt);
            float vCoef = math::pow(0.98f, dt);

            vel = math::vec3(vel.x * hCoef, vel.y * hCoef, vel.z * vCoef);
            math::vec3 hv = math::vec3(vel.x, vel.y, 0.0f);
            const float DV = 5.0f * dt;
            if (math::length(hv) > DV)
				hv = math::normalize(hv) * DV;

			vel -= hv;

            math::vec3 dPos = vel * dt;
            math::vec3 playerSize = math::vec3(0.6f, 0.6f, 1.7f);
            math::vec3 eyeOffset = math::vec3(0.3f, 0.3f, 1.6f);
            math::BBox b(camera.position - eyeOffset, playerSize);

            world.move(b, dPos);
            dPos = b.point - (camera.position - eyeOffset);
            vel = dPos / dt;

            camera.position += dPos;
        }

        world.dayNightLightCoef = 0.01f + 0.99*(1.0 + std::cos(globalT / 10.0))/2.0;
        //std::cout << "dayNightLightCoef " << world.dayNightLightCoef << std::endl;

        {
			checkGLError("1");
            //boost::timer::auto_cpu_timer t;
            glBindFramebuffer ( GL_FRAMEBUFFER, frameBuffer );
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture, 0);
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, frameBufferLightTexture, 0);
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture, 0);

            //glDrawBuffer(GL_COLOR_ATTACHMENT0);
            //glReadBuffer(GL_COLOR_ATTACHMENT0);
            //GLenum drawBufs[] = {GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT1};

            //glDrawBuffers(1, drawBufs);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            glClearColor(0.0,0.0,0.0,0.0);
            glClear(GL_COLOR_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);

			checkGLError("2");
            //glEnable(GL_POLYGON_OFFSET_FILL);
            world.render(camera);
            //glDisable(GL_POLYGON_OFFSET_FILL);

            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, 0, 0);
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, 0, 0);
            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, frameBufferTexture2, 0);

            //glFlush();
            lightMultiplier = 1.0/std::pow(0.05f, 1.0f - world.getMaxLightNearPoint(camera.position));
            if (lightMultiplier > 10.0)
                lightMultiplier = 10.0;
            if (lightMultiplier < 1.0)
                lightMultiplier = 1.0;

			lightMultiplier = lightMultiplierSmoothing.set(lightMultiplier, dt);

			float skyLightMultiplier = std::pow(0.05f, 1.0f - world.dayNightLightCoef);
            glClearColor(0.5 * lightMultiplier * skyLightMultiplier, 0.7 * lightMultiplier * skyLightMultiplier, 1.0 * lightMultiplier * skyLightMultiplier, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glUseProgram(quadShader.program);
            glBindVertexArray(quadVao);

            checkGLError("3");

            if (quadShader.uniforms.count("textureSampler"))
            {
                glUniform1i(quadShader.uniforms["textureSampler"], 0);
                glActiveTexture(GL_TEXTURE0 + 0);
                glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture);
            }

            if (quadShader.uniforms.count("lightSampler"))
            {
                glUniform1i(quadShader.uniforms["lightSampler"], 1);
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferLightTexture);
            }

            if (quadShader.uniforms.count("materialSampler"))
            {
                glUniform1i(quadShader.uniforms["materialSampler"], 2);
                glActiveTexture(GL_TEXTURE0 + 2);
                glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferMaterialTexture);
            }

            if (quadShader.uniforms.count("blockSampler"))
            {
                glUniform1i(quadShader.uniforms["blockSampler"], 3);
                glActiveTexture(GL_TEXTURE0 + 3);
                glBindTexture(GL_TEXTURE_2D_ARRAY, blockTexture);
            }

            checkGLError("5");

            if (quadShader.uniforms.count("HSColorSampler"))
            {
                //std::cout << "HSColorSampler " << HSColorTexture << std::endl;
                glUniform1i(quadShader.uniforms["HSColorSampler"], 4);
                glActiveTexture(GL_TEXTURE0 + 4);
                glBindTexture(GL_TEXTURE_2D, HSColorTexture);
            }

            checkGLError("6");

            if (quadShader.uniforms.count("lightMultiplier"))
            {
                //std::cout << "lightMultiplier " << lightMultiplier << std::endl;
                glUniform1f(quadShader.uniforms["lightMultiplier"], lightMultiplier);
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

                    //std::cout << "POS " << bpos.x << " " << bpos.y << " " << bpos.z << std::endl;

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

                    if (rClicked)
                    {
						world.putBlock(prevBPos, 3);

						std::vector<AddedBlockLight> addedBlocks;
						std::vector<math::ivec3> removedBlocks;

						static int k = 0;
						k++;
						if (k % 3 == 0)
							addedBlocks.push_back({prevBPos, {0, 20, 0, 0}});
						else if (k % 3 == 1)
							addedBlocks.push_back({prevBPos, {0, 0, 20, 0}});
						else
							addedBlocks.push_back({prevBPos, {0, 0, 0, 20}});

						world.updateLight<LIGHT_R>(addedBlocks, removedBlocks);
						world.updateLight<LIGHT_G>(addedBlocks, removedBlocks);
						world.updateLight<LIGHT_B>(addedBlocks, removedBlocks);

						std::cout << "ADDB " << eucModChunk(prevBPos.x) << " " << eucModChunk(prevBPos.y) << " " << eucModChunk(prevBPos.z) << std::endl;
                    }

                    if (lClicked)
                    {
						world.putBlock(bpos, 0);

						std::vector<AddedBlockLight> addedBlocks;
						std::vector<math::ivec3> removedBlocks;
						removedBlocks.push_back(bpos);
						world.updateLight<LIGHT_R>(addedBlocks, removedBlocks);
						world.updateLight<LIGHT_G>(addedBlocks, removedBlocks);
						world.updateLight<LIGHT_B>(addedBlocks, removedBlocks);
                    }
                }
            }

			checkGLError("9");

            glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);
            glBindFramebuffer ( GL_FRAMEBUFFER, 0);

            glDrawBuffer(GL_BACK);
            glReadBuffer(GL_BACK);

            //glClear(GL_DEPTH_BUFFER_BIT);

            glUseProgram(quadShader2.program);
            glBindVertexArray(quadVao);
            if (quadShader2.uniforms.count("textureSampler"))
            {
                glUniform1i(quadShader2.uniforms["textureSampler"], 0);
                glActiveTexture(GL_TEXTURE0 + 0);
                glBindTexture(GL_TEXTURE_RECTANGLE, frameBufferTexture2);
            }

            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glUseProgram(0);

            glBindTexture(GL_TEXTURE_2D, 0);

            glFinish();
        }

        SDL_GL_SwapWindow(window);
        //SDL_Delay(20);

    }

    SDL_GL_DeleteContext(glContext);
    SDL_Quit();
    return 0;
}

