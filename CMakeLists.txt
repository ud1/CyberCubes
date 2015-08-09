cmake_minimum_required(VERSION 2.8)

project(cubercubes)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

add_executable(cubercubes
main.cpp
src/Atlas.cpp
src/Block.cpp
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
)

find_package(Lua51 REQUIRED) 

include_directories(include ${LUA_INCLUDE_DIR})

install(TARGETS cubercubes RUNTIME DESTINATION bin)

target_link_libraries(cubercubes
    GL GLEW SDL2 SDL2_image boost_system boost_timer noise ${LUA_LIBRARIES} tolua++5.1 z sqlite3
)
