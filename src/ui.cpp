#include "ui.h"
#include "imgui.h"
#include "canvas.h"
#include "graphics.h"
#include <cstdio>

static Canvas* g_canvas = nullptr;
static char g_pixelInfo[128] = "";

void UI_SetCanvas(Canvas* canvas) {
    g_canvas = canvas;
}

void UI_SetPixelInfo(const char* info) {
    snprintf(g_pixelInfo, sizeof(g_pixelInfo), "%s", info);
}

void UI_Render(void) {
    ImGui::Begin("Painel de Controle");
    
    if (ImGui::Button("Carregar Imagem")) {
        if (g_canvas) {
            Canvas_LoadImage(g_canvas, "image.jpg");
        }
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Tons de Cinza")) {
        if (g_canvas) {
            Graphics_ApplyGrayscale(g_canvas);
        }
    }
    
    if (ImGui::Button("Negativo")) {
        if (g_canvas) {
            Graphics_ApplyNegative(g_canvas);
        }
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Canal R")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_R);
        }
    }
    
    if (ImGui::Button("Canal G")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_G);
        }
    }
    
    if (ImGui::Button("Canal B")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_B);
        }
    }
    
    ImGui::Separator();
    
    ImGui::Text("Inspetor de Pixels:");
    ImGui::Text("%s", g_pixelInfo);
    
    ImGui::End();
}