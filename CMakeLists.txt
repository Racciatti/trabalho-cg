# Versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

# Nome do projeto. IMPORTANTE: Adicionado CXX para compilar os arquivos .cpp do Dear ImGui
project(ProjetoCG C CXX)

# Define os padrões das linguagens
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra o pacote SDL2
find_package(SDL2 REQUIRED)

# Adiciona os diretórios de include para o seu projeto e para o Dear ImGui
include_directories(
    include
    deps/imgui                # Para encontrar "imgui.h"
    deps/imgui/backends       # Para encontrar os arquivos de integração
)

# Encontra os arquivos de código-fonte da sua aplicação
file(GLOB APP_SOURCES "src/*.c")

# Lista os arquivos de código-fonte necessários do Dear ImGui
set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_sdl2.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer2.cpp # Use este se estiver usando SDL_Renderer
)

# Cria o executável com os SEUS fontes E os fontes do ImGui
add_executable(seu_executavel ${APP_SOURCES} ${IMGUI_SOURCES})

# Vincula apenas a biblioteca SDL2. O código do ImGui já está compilado no executável.
target_link_libraries(seu_executavel PRIVATE SDL2::SDL2)
