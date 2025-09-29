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
#include <cmath>
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
static int g_selectedFillAlgorithm = 0;
static float g_translate[3] = {0.0f, 0.0f, 0.0f};
static float g_scale[3] = {1.0f, 1.0f, 1.0f};
static int g_rotationAxis = 0;
static float g_rotationAngle = 0.0f;
static Object3D* g_object3D = nullptr;
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
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Painel de Controle");
    
    // Make the window scrollable
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
    
    // Left Column
    if (ImGui::BeginChild("LeftColumn", ImVec2(180, 0), true)) {
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
    
    }
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Right Column
    if (ImGui::BeginChild("RightColumn", ImVec2(180, 0), true)) {
        ImGui::Text("Transformações 3D:");
    
    // Translation controls
    ImGui::Text("Translação:");
    ImGui::SliderFloat("Tx", &g_translate[0], -200.0f, 200.0f);
    ImGui::SliderFloat("Ty", &g_translate[1], -200.0f, 200.0f);
    ImGui::SliderFloat("Tz", &g_translate[2], -200.0f, 200.0f);
    
    // Scale controls
    ImGui::Text("Escala:");
    ImGui::SliderFloat("Sx", &g_scale[0], 0.1f, 3.0f);
    ImGui::SliderFloat("Sy", &g_scale[1], 0.1f, 3.0f);
    ImGui::SliderFloat("Sz", &g_scale[2], 0.1f, 3.0f);
    
    // Rotation controls
    ImGui::Text("Rotação:");
    ImGui::RadioButton("Eixo X", &g_rotationAxis, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Eixo Y", &g_rotationAxis, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Eixo Z", &g_rotationAxis, 2);
    ImGui::SliderFloat("Ângulo", &g_rotationAngle, 0.0f, 360.0f);
    
    // Apply transformation button
    if (ImGui::Button("Aplicar Transformação")) {
        if (g_object3D == nullptr) {
            // Create cube if it doesn't exist
            g_object3D = Graphics_CreateCube();
        }
        
        // Clear canvas
        if (g_canvas) {
            Canvas_Clear(g_canvas, 0xFF000000); // Clear to black
        }
        
        // Create transformation matrix
        Matrix4x4 transform = Matrix4x4_Identity();
        
        // Apply translation
        Matrix4x4 translation = Matrix4x4_Translation(g_translate[0], g_translate[1], g_translate[2]);
        transform = Matrix4x4_Multiply(transform, translation);
        
        // Apply scale
        Matrix4x4 scale = Matrix4x4_Scale(g_scale[0], g_scale[1], g_scale[2]);
        transform = Matrix4x4_Multiply(transform, scale);
        
        // Apply rotation
        Matrix4x4 rotation = Matrix4x4_Identity();
        if (g_rotationAxis == 0) {
            rotation = Matrix4x4_RotationX(g_rotationAngle * M_PI / 180.0f);
        } else if (g_rotationAxis == 1) {
            rotation = Matrix4x4_RotationY(g_rotationAngle * M_PI / 180.0f);
        } else {
            rotation = Matrix4x4_RotationZ(g_rotationAngle * M_PI / 180.0f);
        }
        transform = Matrix4x4_Multiply(transform, rotation);
        
        // Render object
        if (g_canvas) {
            Graphics_RenderObject3D(g_canvas, g_object3D, transform, 0xFFFFFFFF);
            g_textureNeedsUpdate = true;
        }
    }
    
    ImGui::Separator();
    
    // Cohen-Sutherland Line Clipping
    ImGui::Text("Recorte de Linhas (Cohen-Sutherland):");
    if (ImGui::Button("Definir Janela de Recorte")) {
        UI_StartClippingMode();
    }
    ImGui::TextWrapped("1. Clique no botão acima");
    ImGui::TextWrapped("2. Clique em dois cantos opostos para definir a janela");
    ImGui::TextWrapped("3. Arraste para desenhar uma linha que será recortada");
    
    ImGui::Separator();
    
    ImGui::Text("Preenchimento de Polígonos:");
    if (ImGui::Button("Desenhar Polígono")) {
        UI_StartPolygonDrawing();
    }
    ImGui::Text("Algoritmo de Preenchimento:");
    ImGui::RadioButton("Flood Fill 4", &g_selectedFillAlgorithm, 0);
    ImGui::RadioButton("Flood Fill 8", &g_selectedFillAlgorithm, 1);
    ImGui::RadioButton("Scan-line", &g_selectedFillAlgorithm, 2);
    
    if (ImGui::Button("Preencher Polígono (Scan-line)")) {
        UI_FillPolygonScanline();
    }
    
    ImGui::TextWrapped("1. Clique em 'Desenhar Polígono'");
    ImGui::TextWrapped("2. Clique com botão esquerdo para adicionar vértices");
    ImGui::TextWrapped("3. Clique com botão direito para fechar o polígono");
    ImGui::TextWrapped("4. Para Flood Fill: clique dentro do polígono");
    ImGui::TextWrapped("5. Para Scan-line: use o botão acima");
    
    }
    ImGui::EndChild();
    
    }
    ImGui::EndChild();
    ImGui::End();
    
    RenderFileDialog();
}

int UI_GetSelectedFillAlgorithm(void) {
    return g_selectedFillAlgorithm;
}

static Polygon* g_uiCurrentPolygon = nullptr;

void UI_SetCurrentPolygon(Polygon* poly) {
    g_uiCurrentPolygon = poly;
}

void UI_FillPolygonScanline(void) {
    if (g_uiCurrentPolygon && g_uiCurrentPolygon->vertex_count >= 3) {
        if (g_canvas) {
            Color fillColor = {0, 255, 0, 255};
            Graphics_Fill_Scanline(g_canvas, g_uiCurrentPolygon, fillColor);
            g_textureNeedsUpdate = true;
        }
    }
}

void UI_Cleanup(void) {
    if (g_object3D != nullptr) {
        Graphics_DestroyObject3D(g_object3D);
        g_object3D = nullptr;
    }
}