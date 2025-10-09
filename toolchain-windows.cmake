# Define o sistema alvo como Windows
set(CMAKE_SYSTEM_NAME Windows)

# Especifica os compiladores de C e C++
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Especifica o compilador de recursos (para ícones, etc.)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Ajusta o modo de busca para encontrar as bibliotecas e includes do MinGW-w64
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)