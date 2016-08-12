cmake_minimum_required(VERSION 2.8)

project(cubercubes)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x -O2)
endif()

add_executable(cubercubes
main.cpp
src/Block.cpp
src/BlockRenderer.cpp
src/Camera.cpp
src/Chunk.cpp
src/colorTransform.cpp
src/Frustum.cpp
src/GLQueryPool.cpp
src/Player.cpp
src/RenderData.cpp
src/Shader.cpp
src/World.cpp
src/WorldProvider.cpp
src/toluaGen/Block.cpp
src/Inventory.cpp
src/SoundSystem.cpp
src/Input.cpp
src/Math.cpp
src/ModelRenderer.cpp
src/ModelTextures.cpp
src/GeomOcclusitionCulling.cpp

src/models.cpp
src/models/ModelWolf.cpp
src/models/ModelChest.cpp

ext/nanovg/src/nanovg.c
)

find_package(Lua51 REQUIRED)
find_package(OpenAL REQUIRED)

include_directories(include ${LUA_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR} ext/nanovg/src)

install(TARGETS cubercubes RUNTIME DESTINATION bin)

target_link_libraries(cubercubes
    GL GLEW SDL2 SDL2_image boost_system boost_timer noise ${LUA_LIBRARIES} tolua++5.1 z sqlite3 ${OPENAL_LIBRARY} ogg vorbis vorbisfile
)
