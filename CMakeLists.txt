cmake_minimum_required(VERSION 3.8)
project(Nabla2D VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Sources
set(SOURCES
  src/main.cpp
  src/logger.cpp
  src/game.cpp
  src/renderer/renderer.cpp
  src/renderer/SDL/sdlglrenderer.cpp
  src/renderer/OpenGL/glshader.cpp
  src/renderer/OpenGL/glvertex.cpp
  src/renderer/OpenGL/gldata.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Static link libgcc and libstdc++
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -std=gnu++17)
endif()

# Warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})