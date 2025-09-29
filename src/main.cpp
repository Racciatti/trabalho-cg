#include <SDL2/SDL.h>
#include <stdio.h>

// Headers do Dear ImGui
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "canvas.h"
#include "ui.h"

int main(int, char**) {
    // --- 1. Inicialização do SDL ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // --- 2. Criação da Janela e Renderizador SDL ---
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Ferramenta de Computação Gráfica", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }

    // --- 3. Setup do Dear ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup do estilo
    ImGui::StyleColorsDark();

    // Setup dos Backends de Renderização
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // --- Criação do Canvas ---
    Canvas* canvas = Canvas_Create(640, 480);
    if (!canvas) {
        printf("Error: Could not create canvas!\n");
        return -1;
    }
    UI_SetCanvas(canvas);

    SDL_Texture* canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_STREAMING,
                                                   canvas->width, canvas->height);

    // --- 4. Loop Principal da Aplicação ---
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Passa o evento para o ImGui
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // --- Inspetor de Pixels ---
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        int canvasX = mouseX - 50;
        int canvasY = mouseY - 50;
        
        if (canvasX >= 0 && canvasX < canvas->width && canvasY >= 0 && canvasY < canvas->height) {
            uint32_t pixel = Canvas_GetPixel(canvas, canvasX, canvasY);
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t b = pixel & 0xFF;
            
            char pixelInfo[128];
            snprintf(pixelInfo, sizeof(pixelInfo), "(%d, %d) - R: %d, G: %d, B: %d", canvasX, canvasY, r, g, b);
            UI_SetPixelInfo(pixelInfo);
        } else {
            UI_SetPixelInfo("Cursor fora do canvas");
        }

        // --- Inicia um novo frame do ImGui ---
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // --- Desenho da UI ---
        UI_Render();

        // --- Renderização ---
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        // --- Renderização do Canvas ---
        SDL_UpdateTexture(canvasTexture, NULL, canvas->pixels, canvas->width * sizeof(uint32_t));
        SDL_Rect canvasRect = {50, 50, canvas->width, canvas->height};
        SDL_RenderCopy(renderer, canvasTexture, NULL, &canvasRect);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    // --- 5. Limpeza ---
    SDL_DestroyTexture(canvasTexture);
    Canvas_Destroy(canvas);
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}