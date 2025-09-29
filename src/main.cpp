#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

// Headers do Dear ImGui
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "canvas.h"
#include "ui.h"
#include "graphics.h"

// Interaction modes for Cohen-Sutherland clipping
typedef enum {
    MODE_NONE,
    MODE_DEFINING_WINDOW,
    MODE_DEFINING_LINE,
    MODE_DRAWING_POLYGON,
    MODE_FILLING_POLYGON
} InteractionMode;

// Global variables for clipping interaction
static InteractionMode g_currentMode = MODE_NONE;
static Rect g_clipWindow;
static int g_windowClickCount = 0;
static int g_isDefiningLine = 0;
static int g_lineStartX = 0, g_lineStartY = 0;
static Polygon* g_currentPolygon = nullptr;

// Forward declarations
void StartWindowDefinition();
void StartPolygonDrawing();
void HandlePolygonDrawing(Canvas* canvas, int x, int y, int isRightClick);

// Function to be called from UI to start clipping mode
void UI_StartClippingMode(void) {
    StartWindowDefinition();
}

// Function to be called from UI to start polygon drawing mode
void UI_StartPolygonDrawing(void) {
    StartPolygonDrawing();
}

// Function to start window definition mode
void StartWindowDefinition() {
    g_currentMode = MODE_DEFINING_WINDOW;
    g_windowClickCount = 0;
    printf("Mode: Defining clipping window - click two opposite corners\n");
}

// Function to handle window definition clicks
void HandleWindowDefinition(Canvas* canvas, int x, int y) {
    if (g_windowClickCount == 0) {
        // First click - store the starting corner
        g_clipWindow.xmin = x;
        g_clipWindow.ymin = y;
        g_windowClickCount = 1;
        printf("First corner selected: (%d, %d)\n", x, y);
    } else if (g_windowClickCount == 1) {
        // Second click - complete the window definition
        g_clipWindow.xmax = x;
        g_clipWindow.ymax = y;
        
        // Ensure min/max are correct
        if (g_clipWindow.xmin > g_clipWindow.xmax) {
            int temp = g_clipWindow.xmin;
            g_clipWindow.xmin = g_clipWindow.xmax;
            g_clipWindow.xmax = temp;
        }
        if (g_clipWindow.ymin > g_clipWindow.ymax) {
            int temp = g_clipWindow.ymin;
            g_clipWindow.ymin = g_clipWindow.ymax;
            g_clipWindow.ymax = temp;
        }
        
        // Draw the clipping window
        Graphics_DrawRect(canvas, g_clipWindow, 0xFF000000); // Black rectangle
        
        // Switch to line definition mode
        g_currentMode = MODE_DEFINING_LINE;
        printf("Window defined: (%d,%d) to (%d,%d). Now draw a line to clip.\n", 
               g_clipWindow.xmin, g_clipWindow.ymin, g_clipWindow.xmax, g_clipWindow.ymax);
    }
}

// Function to handle line definition
void HandleLineDefinition(Canvas* canvas, int x, int y, int mouseDown) {
    if (mouseDown && !g_isDefiningLine) {
        // Start defining line
        g_isDefiningLine = 1;
        g_lineStartX = x;
        g_lineStartY = y;
    } else if (!mouseDown && g_isDefiningLine) {
        // Finish defining line and apply clipping
        g_isDefiningLine = 0;
        
        Line line = {g_lineStartX, g_lineStartY, x, y};
        
        // Draw original line in black
        Graphics_DrawLine_Simple(canvas, line.x1, line.y1, line.x2, line.y2, 0xFF000000);
        
        // Apply Cohen-Sutherland clipping
        Line clippedLine = line; // Make a copy
        if (Graphics_ClipLine_CohenSutherland(&clippedLine, g_clipWindow)) {
            // Draw clipped line in black (same as original for consistency)
            Graphics_DrawLine_Simple(canvas, clippedLine.x1, clippedLine.y1, 
                                   clippedLine.x2, clippedLine.y2, 0xFF000000);
            printf("Line clipped from (%d,%d)-(%d,%d) to (%d,%d)-(%d,%d)\n",
                   line.x1, line.y1, line.x2, line.y2,
                   clippedLine.x1, clippedLine.y1, clippedLine.x2, clippedLine.y2);
        } else {
            printf("Line completely outside clipping window\n");
        }
        
        // Reset mode
        g_currentMode = MODE_NONE;
    }
}

void StartPolygonDrawing() {
    g_currentMode = MODE_DRAWING_POLYGON;
    if (g_currentPolygon) {
        Graphics_DestroyPolygon(g_currentPolygon);
    }
    g_currentPolygon = Graphics_CreatePolygon(100);
    UI_SetCurrentPolygon(g_currentPolygon);
    printf("Mode: Drawing polygon - left click to add vertices, right click to close\n");
}

void HandlePolygonDrawing(Canvas* canvas, int x, int y, int isRightClick) {
    if (isRightClick) {
        if (g_currentPolygon && g_currentPolygon->vertex_count >= 3) {
            Graphics_DrawLine_Bresenham(canvas, 
                g_currentPolygon->vertices[g_currentPolygon->vertex_count-1].x,
                g_currentPolygon->vertices[g_currentPolygon->vertex_count-1].y,
                g_currentPolygon->vertices[0].x, g_currentPolygon->vertices[0].y,
                0xFF000000);
            g_currentMode = MODE_FILLING_POLYGON;
            printf("Polygon closed. Ready for filling.\n");
        }
    } else {
        Graphics_AddVertexToPolygon(g_currentPolygon, x, y);
        if (g_currentPolygon->vertex_count > 1) {
            Graphics_DrawLine_Bresenham(canvas,
                g_currentPolygon->vertices[g_currentPolygon->vertex_count-2].x,
                g_currentPolygon->vertices[g_currentPolygon->vertex_count-2].y,
                x, y, 0xFF000000);
        }
        Canvas_SetPixel(canvas, x, y, 0xFF000000);
    }
}

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
    
    // Initialize canvas with white background
    Canvas_Clear(canvas, 0xFFFFFFFF);
    
    UI_SetCanvas(canvas);

    SDL_Texture* canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                                   SDL_TEXTUREACCESS_STREAMING,
                                                   canvas->width, canvas->height);
    
    // Initial texture update to show the blank canvas
    SDL_UpdateTexture(canvasTexture, NULL, canvas->pixels, canvas->width * sizeof(uint32_t));

    // Line drawing state
    bool waitingForFirstClick = true;
    int firstClickX = 0, firstClickY = 0;
    
    // Circle drawing state
    bool drawingCircle = false;
    int circleCenterX = 0, circleCenterY = 0;

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
            
            // Handle mouse clicks for line drawing and clipping
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                ImGuiIO& io = ImGui::GetIO();
                if (!io.WantCaptureMouse) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    int canvasX = mouseX - 50;
                    int canvasY = mouseY - 50;
                    
                    if (canvasX >= 0 && canvasX < canvas->width && canvasY >= 0 && canvasY < canvas->height) {
                        // Handle clipping modes first
                        if (g_currentMode == MODE_DEFINING_WINDOW) {
                            HandleWindowDefinition(canvas, canvasX, canvasY);
                            UI_TriggerTextureUpdate();
                        } else if (g_currentMode == MODE_DEFINING_LINE) {
                            HandleLineDefinition(canvas, canvasX, canvasY, 1); // mouseDown = 1
                            UI_TriggerTextureUpdate();
                        } else if (g_currentMode == MODE_DRAWING_POLYGON) {
                            HandlePolygonDrawing(canvas, canvasX, canvasY, 0);
                            UI_TriggerTextureUpdate();
                        } else if (g_currentMode == MODE_FILLING_POLYGON) {
                            int fillAlgorithm = UI_GetSelectedFillAlgorithm();
                            if (fillAlgorithm == 0 || fillAlgorithm == 1) {
                                uint32_t targetColorUint = Canvas_GetPixel(canvas, canvasX, canvasY);
                                Color targetColor = Graphics_Uint32ToColor(targetColorUint);
                                Color fillColor = {255, 0, 0, 255};
                                int connectivity = (fillAlgorithm == 0) ? 4 : 8;
                                Graphics_Fill_FloodFill(canvas, canvasX, canvasY, targetColor, fillColor, connectivity);
                                UI_TriggerTextureUpdate();
                            }
                        } else {
                            // Normal line drawing mode
                            if (waitingForFirstClick) {
                                firstClickX = canvasX;
                                firstClickY = canvasY;
                                waitingForFirstClick = false;
                            } else {
                                int algorithm = UI_GetSelectedLineAlgorithm();
                                uint32_t color = 0xFF000000;
                                
                                switch (algorithm) {
                                    case 0:
                                        Graphics_DrawLine_GeneralEquation(canvas, firstClickX, firstClickY, canvasX, canvasY, color);
                                        break;
                                    case 1:
                                        Graphics_DrawLine_Parametric(canvas, firstClickX, firstClickY, canvasX, canvasY, color);
                                        break;
                                    case 2:
                                        Graphics_DrawLine_Bresenham(canvas, firstClickX, firstClickY, canvasX, canvasY, color);
                                        break;
                                }
                                
                                UI_TriggerTextureUpdate();
                                waitingForFirstClick = true;
                            }
                        }
                    }
                }
            }
            
            // Handle right mouse button for circle drawing and polygon closing
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                ImGuiIO& io = ImGui::GetIO();
                if (!io.WantCaptureMouse) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    int canvasX = mouseX - 50;
                    int canvasY = mouseY - 50;
                    
                    if (canvasX >= 0 && canvasX < canvas->width && canvasY >= 0 && canvasY < canvas->height) {
                        if (g_currentMode == MODE_DRAWING_POLYGON) {
                            HandlePolygonDrawing(canvas, canvasX, canvasY, 1);
                            UI_TriggerTextureUpdate();
                        } else {
                            drawingCircle = true;
                            circleCenterX = canvasX;
                            circleCenterY = canvasY;
                        }
                    }
                }
            }
            
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
                if (drawingCircle) {
                    ImGuiIO& io = ImGui::GetIO();
                    if (!io.WantCaptureMouse) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;
                        int canvasX = mouseX - 50;
                        int canvasY = mouseY - 50;
                        
                        if (canvasX >= 0 && canvasX < canvas->width && canvasY >= 0 && canvasY < canvas->height) {
                            int dx = canvasX - circleCenterX;
                            int dy = canvasY - circleCenterY;
                            int radius = (int)(sqrt(dx * dx + dy * dy) + 0.5f);
                            
                            if (radius > 0) {
                                int algorithm = UI_GetSelectedCircleAlgorithm();
                                uint32_t color = 0xFF000000;
                                
                                switch (algorithm) {
                                    case 0:
                                        Graphics_DrawCircle_Explicit(canvas, circleCenterX, circleCenterY, radius, color);
                                        break;
                                    case 1:
                                        Graphics_DrawCircle_Parametric(canvas, circleCenterX, circleCenterY, radius, color);
                                        break;
                                    case 2:
                                        Graphics_DrawCircle_IncrementalRotation(canvas, circleCenterX, circleCenterY, radius, color);
                                        break;
                                    case 3:
                                        Graphics_DrawCircle_Bresenham(canvas, circleCenterX, circleCenterY, radius, color);
                                        break;
                                }
                                
                                UI_TriggerTextureUpdate();
                            }
                        }
                    }
                    drawingCircle = false;
                }
            }
            
            // Handle mouse button up for line definition in clipping mode
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                if (g_currentMode == MODE_DEFINING_LINE) {
                    ImGuiIO& io = ImGui::GetIO();
                    if (!io.WantCaptureMouse) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;
                        int canvasX = mouseX - 50;
                        int canvasY = mouseY - 50;
                        
                        if (canvasX >= 0 && canvasX < canvas->width && canvasY >= 0 && canvasY < canvas->height) {
                            HandleLineDefinition(canvas, canvasX, canvasY, 0); // mouseDown = 0
                            UI_TriggerTextureUpdate();
                        }
                    }
                }
            }
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
        if (UI_TextureNeedsUpdate()) {
            // Recreate texture if dimensions changed
            int currentWidth, currentHeight;
            SDL_QueryTexture(canvasTexture, NULL, NULL, &currentWidth, &currentHeight);
            
            if (currentWidth != canvas->width || currentHeight != canvas->height) {
                SDL_DestroyTexture(canvasTexture);
                canvasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_STREAMING,
                                                 canvas->width, canvas->height);
            }
            
            SDL_UpdateTexture(canvasTexture, NULL, canvas->pixels, canvas->width * sizeof(uint32_t));
        }
        SDL_Rect canvasRect = {50, 50, canvas->width, canvas->height};
        SDL_RenderCopy(renderer, canvasTexture, NULL, &canvasRect);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    // --- 5. Limpeza ---
    SDL_DestroyTexture(canvasTexture);
    Canvas_Destroy(canvas);
    
    if (g_currentPolygon) {
        Graphics_DestroyPolygon(g_currentPolygon);
    }
    
    UI_Cleanup();
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}