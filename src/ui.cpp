#include "ui.h"
#include "imgui.h"
#include "canvas.h"
#include "graphics.h"
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>

static Canvas* g_canvas = nullptr;
static char g_pixelInfo[128] = "";
static char g_imagePath[256] = "";
static char g_statusMessage[256] = "";
static char g_currentDir[512] = ".";
static bool g_showFileDialog = false;
static bool g_textureNeedsUpdate = false;
static int g_selectedLineAlgorithm = 0;
static int g_selectedCircleAlgorithm = 0;
static std::vector<std::string> g_fileList;
static std::vector<std::string> g_dirList;

void UI_SetCanvas(Canvas* canvas) {
    g_canvas = canvas;
}

void UI_SetPixelInfo(const char* info) {
    snprintf(g_pixelInfo, sizeof(g_pixelInfo), "%s", info);
}

bool UI_TextureNeedsUpdate() {
    bool needsUpdate = g_textureNeedsUpdate;
    g_textureNeedsUpdate = false;
    return needsUpdate;
}

int UI_GetSelectedLineAlgorithm() {
    return g_selectedLineAlgorithm;
}

void UI_TriggerTextureUpdate() {
    g_textureNeedsUpdate = true;
}

int UI_GetSelectedCircleAlgorithm() {
    return g_selectedCircleAlgorithm;
}

static void ScanDirectory(const char* path) {
    g_fileList.clear();
    g_dirList.clear();
    
    DIR* dir = opendir(path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.' && strlen(entry->d_name) == 1) continue;
        
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        struct stat statbuf;
        if (stat(fullPath, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                g_dirList.push_back(std::string(entry->d_name));
            } else {
                const char* ext = strrchr(entry->d_name, '.');
                if (ext && (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0 || 
                           strcasecmp(ext, ".png") == 0 || strcasecmp(ext, ".bmp") == 0)) {
                    g_fileList.push_back(std::string(entry->d_name));
                }
            }
        }
    }
    closedir(dir);
    
    std::sort(g_dirList.begin(), g_dirList.end());
    std::sort(g_fileList.begin(), g_fileList.end());
}

static void RenderFileDialog() {
    if (!g_showFileDialog) return;
    
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Selecionar Imagem", &g_showFileDialog)) {
        ImGui::Text("Diretório atual: %s", g_currentDir);
        
        if (ImGui::Button("..") && strcmp(g_currentDir, "/") != 0) {
            char* lastSlash = strrchr(g_currentDir, '/');
            if (lastSlash && lastSlash != g_currentDir) {
                *lastSlash = '\0';
            } else if (lastSlash == g_currentDir) {
                strcpy(g_currentDir, "/");
            }
            ScanDirectory(g_currentDir);
        }
        
        ImGui::Separator();
        
        if (ImGui::BeginChild("FileList", ImVec2(0, -30))) {
            for (const auto& dir : g_dirList) {
                if (ImGui::Selectable(("📁 " + dir).c_str())) {
                    if (strcmp(g_currentDir, "/") == 0) {
                        snprintf(g_currentDir, sizeof(g_currentDir), "/%s", dir.c_str());
                    } else {
                        snprintf(g_currentDir, sizeof(g_currentDir), "%s/%s", g_currentDir, dir.c_str());
                    }
                    ScanDirectory(g_currentDir);
                }
            }
            
            for (const auto& file : g_fileList) {
                if (ImGui::Selectable(("🖼️ " + file).c_str())) {
                    if (strcmp(g_currentDir, "/") == 0) {
                        snprintf(g_imagePath, sizeof(g_imagePath), "/%s", file.c_str());
                    } else {
                        snprintf(g_imagePath, sizeof(g_imagePath), "%s/%s", g_currentDir, file.c_str());
                    }
                    g_showFileDialog = false;
                }
            }
        }
        ImGui::EndChild();
        
        if (ImGui::Button("Cancelar")) {
            g_showFileDialog = false;
        }
    }
    ImGui::End();
}

void UI_Render(void) {
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Painel de Controle");
    
    ImGui::Text("Carregamento de Imagem:");
    
    if (ImGui::Button("Selecionar Imagem...")) {
        g_showFileDialog = true;
        ScanDirectory(g_currentDir);
    }
    
    if (strlen(g_imagePath) > 0) {
        ImGui::Text("Selecionado: %s", g_imagePath);
        
        if (ImGui::Button("Carregar Imagem")) {
            if (g_canvas) {
                if (Canvas_LoadImage(g_canvas, g_imagePath)) {
                    snprintf(g_statusMessage, sizeof(g_statusMessage), "Imagem carregada com sucesso!");
                    g_textureNeedsUpdate = true;
                } else {
                    snprintf(g_statusMessage, sizeof(g_statusMessage), "Erro ao carregar a imagem");
                }
            }
        }
    }
    
    if (strlen(g_statusMessage) > 0) {
        ImGui::Text("%s", g_statusMessage);
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Tons de Cinza")) {
        if (g_canvas) {
            Graphics_ApplyGrayscale(g_canvas);
            g_textureNeedsUpdate = true;
        }
    }
    
    if (ImGui::Button("Negativo")) {
        if (g_canvas) {
            Graphics_ApplyNegative(g_canvas);
            g_textureNeedsUpdate = true;
        }
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Canal R")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_R);
            g_textureNeedsUpdate = true;
        }
    }
    
    if (ImGui::Button("Canal G")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_G);
            g_textureNeedsUpdate = true;
        }
    }
    
    if (ImGui::Button("Canal B")) {
        if (g_canvas) {
            Graphics_ApplyChannel(g_canvas, CHANNEL_B);
            g_textureNeedsUpdate = true;
        }
    }
    
    ImGui::Separator();
    
    ImGui::Text("Desenho de Linhas:");
    ImGui::RadioButton("Equação Geral", &g_selectedLineAlgorithm, 0);
    ImGui::RadioButton("Paramétrica", &g_selectedLineAlgorithm, 1);
    ImGui::RadioButton("Bresenham", &g_selectedLineAlgorithm, 2);
    
    ImGui::Separator();
    
    ImGui::Text("Desenho de Circunferências:");
    ImGui::RadioButton("Explícita", &g_selectedCircleAlgorithm, 0);
    ImGui::RadioButton("Paramétrica C", &g_selectedCircleAlgorithm, 1);
    ImGui::RadioButton("Rotação Incremental", &g_selectedCircleAlgorithm, 2);
    ImGui::RadioButton("Bresenham C", &g_selectedCircleAlgorithm, 3);
    
    ImGui::Text("Clique esquerdo: linhas");
    ImGui::Text("Clique direito: círculos");
    
    ImGui::Separator();
    
    ImGui::Text("Inspetor de Pixels:");
    ImGui::Text("%s", g_pixelInfo);
    
    ImGui::End();
    
    RenderFileDialog();
}