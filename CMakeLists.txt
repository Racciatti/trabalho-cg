# Versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

# O projeto agora é C e CXX (C++)
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
    deps/imgui
    deps/imgui/backends
)

# Lista explícita dos arquivos de código-fonte
set(APP_SOURCES
    src/main.cpp
    src/canvas.c
    src/ui.cpp
)

# Lista os arquivos de código-fonte necessários do Dear ImGui
set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_sdl2.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# Cria o executável com os SEUS fontes E os fontes do ImGui
add_executable(seu_executavel ${APP_SOURCES} ${IMGUI_SOURCES})

# Vincula a biblioteca SDL2
target_link_libraries(seu_executavel PRIVATE SDL2::SDL2)

