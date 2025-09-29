cmake_minimum_required(VERSION 3.16)
project(trabalho-cg)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_RENDERER REQUIRED SDL2_gfx)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

set(SOURCES
    src/main.c
    src/canvas.c
    src/ui.c
    ${IMGUI_SOURCES}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE ${SDL2_CFLAGS_OTHER})