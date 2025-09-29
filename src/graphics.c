#include "graphics.h"
#include "matrix.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Graphics_ApplyGrayscale(Canvas* canvas) {
    if (!canvas || !canvas->pixels) return;
    
    for (int i = 0; i < canvas->width * canvas->height; i++) {
        uint32_t pixel = canvas->pixels[i];
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t a = (pixel >> 24) & 0xFF;
        
        uint8_t gray = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);
        canvas->pixels[i] = (a << 24) | (gray << 16) | (gray << 8) | gray;
    }
}

void Graphics_ApplyNegative(Canvas* canvas) {
    if (!canvas || !canvas->pixels) return;
    
    for (int i = 0; i < canvas->width * canvas->height; i++) {
        uint32_t pixel = canvas->pixels[i];
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t a = (pixel >> 24) & 0xFF;
        
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
        
        canvas->pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
}

void Graphics_ApplyChannel(Canvas* canvas, Channel c) {
    if (!canvas || !canvas->pixels) return;
    
    for (int i = 0; i < canvas->width * canvas->height; i++) {
        uint32_t pixel = canvas->pixels[i];
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        uint8_t a = (pixel >> 24) & 0xFF;
        
        switch (c) {
            case CHANNEL_R:
                g = 0;
                b = 0;
                break;
            case CHANNEL_G:
                r = 0;
                b = 0;
                break;
            case CHANNEL_B:
                r = 0;
                g = 0;
                break;
        }
        
        canvas->pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
}

void Graphics_DrawLine_GeneralEquation(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color) {
    if (!canvas) return;
    
    if (x1 == x2) {
        int start_y = y1 < y2 ? y1 : y2;
        int end_y = y1 > y2 ? y1 : y2;
        for (int y = start_y; y <= end_y; y++) {
            Canvas_SetPixel(canvas, x1, y, color);
        }
        return;
    }
    
    float m = (float)(y2 - y1) / (float)(x2 - x1);
    float b = y1 - m * x1;
    
    int start_x = x1 < x2 ? x1 : x2;
    int end_x = x1 > x2 ? x1 : x2;
    
    for (int x = start_x; x <= end_x; x++) {
        int y = (int)(m * x + b + 0.5f);
        Canvas_SetPixel(canvas, x, y, color);
    }
}

void Graphics_DrawLine_Parametric(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color) {
    if (!canvas) return;
    
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    
    if (steps == 0) {
        Canvas_SetPixel(canvas, x1, y1, color);
        return;
    }
    
    float x_inc = (float)dx / steps;
    float y_inc = (float)dy / steps;
    
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++) {
        Canvas_SetPixel(canvas, (int)(x + 0.5f), (int)(y + 0.5f), color);
        x += x_inc;
        y += y_inc;
    }
}

void Graphics_DrawLine_Bresenham(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color) {
    if (!canvas) return;
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    int x = x1;
    int y = y1;
    
    while (1) {
        Canvas_SetPixel(canvas, x, y, color);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void Graphics_DrawCircle_Explicit(Canvas* canvas, int xc, int yc, int r, uint32_t color) {
    if (!canvas || r <= 0) return;
    
    for (int x = -r; x <= r; x++) {
        int y_squared = r * r - x * x;
        if (y_squared >= 0) {
            int y = (int)(sqrt(y_squared) + 0.5f);
            Canvas_SetPixel(canvas, xc + x, yc + y, color);
            Canvas_SetPixel(canvas, xc + x, yc - y, color);
        }
    }
}

void Graphics_DrawCircle_Parametric(Canvas* canvas, int xc, int yc, int r, uint32_t color) {
    if (!canvas || r <= 0) return;
    
    int steps = 8 * r;
    if (steps < 360) steps = 360;
    
    for (int i = 0; i < steps; i++) {
        float angle = 2.0f * M_PI * i / steps;
        int x = (int)(r * cos(angle) + 0.5f);
        int y = (int)(r * sin(angle) + 0.5f);
        Canvas_SetPixel(canvas, xc + x, yc + y, color);
    }
}

void Graphics_DrawCircle_IncrementalRotation(Canvas* canvas, int xc, int yc, int r, uint32_t color) {
    if (!canvas || r <= 0) return;
    
    int steps = 8 * r;
    if (steps < 360) steps = 360;
    
    float angle_step = 2.0f * M_PI / steps;
    float cos_step = cos(angle_step);
    float sin_step = sin(angle_step);
    
    float x = r;
    float y = 0;
    
    for (int i = 0; i < steps; i++) {
        Canvas_SetPixel(canvas, xc + (int)(x + 0.5f), yc + (int)(y + 0.5f), color);
        
        float new_x = x * cos_step - y * sin_step;
        float new_y = x * sin_step + y * cos_step;
        x = new_x;
        y = new_y;
    }
}

void Graphics_DrawCircle_Bresenham(Canvas* canvas, int xc, int yc, int r, uint32_t color) {
    if (!canvas || r <= 0) return;
    
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    
    while (x <= y) {
        Canvas_SetPixel(canvas, xc + x, yc + y, color);
        Canvas_SetPixel(canvas, xc + x, yc - y, color);
        Canvas_SetPixel(canvas, xc - x, yc + y, color);
        Canvas_SetPixel(canvas, xc - x, yc - y, color);
        Canvas_SetPixel(canvas, xc + y, yc + x, color);
        Canvas_SetPixel(canvas, xc + y, yc - x, color);
        Canvas_SetPixel(canvas, xc - y, yc + x, color);
        Canvas_SetPixel(canvas, xc - y, yc - x, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

Object3D* Graphics_CreateCube() {
    Object3D* cube = malloc(sizeof(Object3D));
    if (!cube) return NULL;
    
    cube->vertex_count = 8;
    cube->vertices = malloc(8 * sizeof(Vec3));
    if (!cube->vertices) {
        free(cube);
        return NULL;
    }
    
    cube->vertices[0] = Vec3_Create(-50, -50, -50);
    cube->vertices[1] = Vec3_Create( 50, -50, -50);
    cube->vertices[2] = Vec3_Create( 50,  50, -50);
    cube->vertices[3] = Vec3_Create(-50,  50, -50);
    cube->vertices[4] = Vec3_Create(-50, -50,  50);
    cube->vertices[5] = Vec3_Create( 50, -50,  50);
    cube->vertices[6] = Vec3_Create( 50,  50,  50);
    cube->vertices[7] = Vec3_Create(-50,  50,  50);
    
    cube->edge_count = 12;
    cube->edges = malloc(2 * cube->edge_count * sizeof(int));
    if (!cube->edges) {
        free(cube->vertices);
        free(cube);
        return NULL;
    }
    
    // Store edges as pairs of vertex indices
    cube->edges[0] = 0; cube->edges[1] = 1;   // Edge 0
    cube->edges[2] = 1; cube->edges[3] = 2;   // Edge 1  
    cube->edges[4] = 2; cube->edges[5] = 3;   // Edge 2
    cube->edges[6] = 3; cube->edges[7] = 0;   // Edge 3
    cube->edges[8] = 4; cube->edges[9] = 5;   // Edge 4
    cube->edges[10] = 5; cube->edges[11] = 6; // Edge 5
    cube->edges[12] = 6; cube->edges[13] = 7; // Edge 6
    cube->edges[14] = 7; cube->edges[15] = 4; // Edge 7
    cube->edges[16] = 0; cube->edges[17] = 4; // Edge 8
    cube->edges[18] = 1; cube->edges[19] = 5; // Edge 9
    cube->edges[20] = 2; cube->edges[21] = 6; // Edge 10
    cube->edges[22] = 3; cube->edges[23] = 7; // Edge 11
    
    return cube;
}

void Graphics_DestroyObject3D(Object3D* object) {
    if (object) {
        free(object->vertices);
        free(object->edges);
        free(object);
    }
}

void Graphics_Translate3D(Object3D* object, float tx, float ty, float tz) {
    if (!object) return;
    
    Matrix4x4 translation = Matrix4x4_Translation(tx, ty, tz);
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(translation, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_Scale3D(Object3D* object, float sx, float sy, float sz) {
    if (!object) return;
    
    Vec3 center = Vec3_Create(0, 0, 0);
    for (int i = 0; i < object->vertex_count; i++) {
        center.x += object->vertices[i].x;
        center.y += object->vertices[i].y;
        center.z += object->vertices[i].z;
    }
    center.x /= object->vertex_count;
    center.y /= object->vertex_count;
    center.z /= object->vertex_count;
    
    Matrix4x4 toOrigin = Matrix4x4_Translation(-center.x, -center.y, -center.z);
    Matrix4x4 scale = Matrix4x4_Scale(sx, sy, sz);
    Matrix4x4 fromOrigin = Matrix4x4_Translation(center.x, center.y, center.z);
    Matrix4x4 transform = Matrix4x4_Multiply(fromOrigin, Matrix4x4_Multiply(scale, toOrigin));
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(transform, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_Rotate3D(Object3D* object, float angle, int axis) {
    if (!object) return;
    
    Vec3 center = Vec3_Create(0, 0, 0);
    for (int i = 0; i < object->vertex_count; i++) {
        center.x += object->vertices[i].x;
        center.y += object->vertices[i].y;
        center.z += object->vertices[i].z;
    }
    center.x /= object->vertex_count;
    center.y /= object->vertex_count;
    center.z /= object->vertex_count;
    
    Matrix4x4 toOrigin = Matrix4x4_Translation(-center.x, -center.y, -center.z);
    Matrix4x4 rotation;
    switch (axis) {
        case 0: rotation = Matrix4x4_RotationX(angle); break;
        case 1: rotation = Matrix4x4_RotationY(angle); break;
        case 2: rotation = Matrix4x4_RotationZ(angle); break;
        default: rotation = Matrix4x4_Identity(); break;
    }
    Matrix4x4 fromOrigin = Matrix4x4_Translation(center.x, center.y, center.z);
    Matrix4x4 transform = Matrix4x4_Multiply(fromOrigin, Matrix4x4_Multiply(rotation, toOrigin));
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(transform, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_RenderObject3D(Canvas* canvas, Object3D* object, Matrix4x4 transform, uint32_t color) {
    if (!canvas || !object) return;
    
    Vec4* transformed_vertices = malloc(object->vertex_count * sizeof(Vec4));
    if (!transformed_vertices) return;
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        transformed_vertices[i] = Matrix4x4_MultiplyVec4(transform, v);
    }
    
    int centerX = canvas->width / 2;
    int centerY = canvas->height / 2;
    
    for (int i = 0; i < object->edge_count; i++) {
        int idxA = object->edges[2*i];
        int idxB = object->edges[2*i+1];
        Vec4 v1 = transformed_vertices[idxA];
        Vec4 v2 = transformed_vertices[idxB];
        int x1 = centerX + (int)v1.x;
        int y1 = centerY + (int)v1.y;
        int x2 = centerX + (int)v2.x;
        int y2 = centerY + (int)v2.y;
        Graphics_DrawLine_Bresenham(canvas, x1, y1, x2, y2, color);
    }
    
    free(transformed_vertices);
}

// Cohen-Sutherland region codes
#define INSIDE 0  // 0000
#define LEFT   1  // 0001
#define RIGHT  2  // 0010
#define BOTTOM 4  // 0100
#define TOP    8  // 1000

// Function to compute region code for a point (x, y)
static int computeCode(int x, int y, Rect window) {
    int code = INSIDE; // initialized as being inside
    
    if (x < window.xmin)      // to the left of window
        code |= LEFT;
    else if (x > window.xmax) // to the right of window
        code |= RIGHT;
    
    if (y < window.ymin)      // below the window
        code |= BOTTOM;
    else if (y > window.ymax) // above the window
        code |= TOP;
        
    return code;
}

// Cohen-Sutherland line clipping algorithm
// Returns 1 if line is visible (possibly clipped), 0 if completely outside
int Graphics_ClipLine_CohenSutherland(Line* line, Rect window) {
    // Compute region codes for both endpoints of the line
    int code1 = computeCode(line->x1, line->y1, window);
    int code2 = computeCode(line->x2, line->y2, window);
    int accept = 0;
    
    while (1) {
        if ((code1 == 0) && (code2 == 0)) {
            // Both endpoints are inside the window - trivial accept
            accept = 1;
            break;
        } else if (code1 & code2) {
            // Both endpoints share an outside region - trivial reject
            break;
        } else {
            // Line needs clipping
            int code_out;
            int x, y;
            
            // At least one endpoint is outside the window, pick it
            if (code1 != 0)
                code_out = code1;
            else
                code_out = code2;
                
            // Find intersection point using formulas:
            // y = y1 + slope * (x - x1), x = x1 + (1/slope) * (y - y1)
            if (code_out & TOP) {
                // Point is above the clip window
                x = line->x1 + (line->x2 - line->x1) * (window.ymax - line->y1) / (line->y2 - line->y1);
                y = window.ymax;
            } else if (code_out & BOTTOM) {
                // Point is below the clip window
                x = line->x1 + (line->x2 - line->x1) * (window.ymin - line->y1) / (line->y2 - line->y1);
                y = window.ymin;
            } else if (code_out & RIGHT) {
                // Point is to the right of clip window
                y = line->y1 + (line->y2 - line->y1) * (window.xmax - line->x1) / (line->x2 - line->x1);
                x = window.xmax;
            } else if (code_out & LEFT) {
                // Point is to the left of clip window
                y = line->y1 + (line->y2 - line->y1) * (window.xmin - line->x1) / (line->x2 - line->x1);
                x = window.xmin;
            }
            
            // Replace the outside point with intersection point
            if (code_out == code1) {
                line->x1 = x;
                line->y1 = y;
                code1 = computeCode(line->x1, line->y1, window);
            } else {
                line->x2 = x;
                line->y2 = y;
                code2 = computeCode(line->x2, line->y2, window);
            }
        }
    }
    
    return accept;
}

// Simple line drawing function (using Bresenham algorithm)
void Graphics_DrawLine_Simple(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color) {
    Graphics_DrawLine_Bresenham(canvas, x1, y1, x2, y2, color);
}

// Draw rectangle outline
void Graphics_DrawRect(Canvas* canvas, Rect rect, uint32_t color) {
    // Draw four sides of the rectangle
    Graphics_DrawLine_Simple(canvas, rect.xmin, rect.ymin, rect.xmax, rect.ymin, color); // Bottom
    Graphics_DrawLine_Simple(canvas, rect.xmax, rect.ymin, rect.xmax, rect.ymax, color); // Right
    Graphics_DrawLine_Simple(canvas, rect.xmax, rect.ymax, rect.xmin, rect.ymax, color); // Top
    Graphics_DrawLine_Simple(canvas, rect.xmin, rect.ymax, rect.xmin, rect.ymin, color); // Left
}

Polygon* Graphics_CreatePolygon(int capacity) {
    Polygon* poly = malloc(sizeof(Polygon));
    if (!poly) return NULL;
    
    poly->vertices = malloc(capacity * sizeof(Point));
    if (!poly->vertices) {
        free(poly);
        return NULL;
    }
    
    poly->vertex_count = 0;
    poly->capacity = capacity;
    return poly;
}

void Graphics_DestroyPolygon(Polygon* poly) {
    if (poly) {
        free(poly->vertices);
        free(poly);
    }
}

void Graphics_AddVertexToPolygon(Polygon* poly, int x, int y) {
    if (poly && poly->vertex_count < poly->capacity) {
        poly->vertices[poly->vertex_count].x = x;
        poly->vertices[poly->vertex_count].y = y;
        poly->vertex_count++;
    }
}

void Graphics_DrawPolygon(Canvas* canvas, Polygon* poly, uint32_t color) {
    if (!poly || poly->vertex_count < 2) return;
    
    for (int i = 0; i < poly->vertex_count - 1; i++) {
        Graphics_DrawLine_Bresenham(canvas,
            poly->vertices[i].x, poly->vertices[i].y,
            poly->vertices[i+1].x, poly->vertices[i+1].y, color);
    }
    
    if (poly->vertex_count >= 3) {
        Graphics_DrawLine_Bresenham(canvas,
            poly->vertices[poly->vertex_count-1].x, poly->vertices[poly->vertex_count-1].y,
            poly->vertices[0].x, poly->vertices[0].y, color);
    }
}

uint32_t Graphics_ColorToUint32(Color color) {
    return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

Color Graphics_Uint32ToColor(uint32_t color) {
    Color c;
    c.a = (color >> 24) & 0xFF;
    c.r = (color >> 16) & 0xFF;
    c.g = (color >> 8) & 0xFF;
    c.b = color & 0xFF;
    return c;
}

typedef struct {
    int x, y;
} FloodFillPoint;

void Graphics_Fill_FloodFill(Canvas* canvas, int x, int y, Color targetColor, Color replacementColor, int connectivity) {
    if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) return;
    
    uint32_t targetColorUint = Graphics_ColorToUint32(targetColor);
    uint32_t replacementColorUint = Graphics_ColorToUint32(replacementColor);
    uint32_t currentPixel = Canvas_GetPixel(canvas, x, y);
    
    if (currentPixel != targetColorUint || targetColorUint == replacementColorUint) return;
    
    int maxStackSize = canvas->width * canvas->height;
    FloodFillPoint* stack = malloc(maxStackSize * sizeof(FloodFillPoint));
    if (!stack) return;
    
    int stackSize = 0;
    stack[stackSize++] = (FloodFillPoint){x, y};
    
    int dx4[] = {0, 1, 0, -1};
    int dy4[] = {-1, 0, 1, 0};
    int dx8[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy8[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    
    int* dx = (connectivity == 4) ? dx4 : dx8;
    int* dy = (connectivity == 4) ? dy4 : dy8;
    int directions = (connectivity == 4) ? 4 : 8;
    
    while (stackSize > 0) {
        FloodFillPoint current = stack[--stackSize];
        
        if (current.x < 0 || current.x >= canvas->width || 
            current.y < 0 || current.y >= canvas->height) continue;
            
        if (Canvas_GetPixel(canvas, current.x, current.y) != targetColorUint) continue;
        
        Canvas_SetPixel(canvas, current.x, current.y, replacementColorUint);
        
        for (int i = 0; i < directions; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            
            if (nx >= 0 && nx < canvas->width && ny >= 0 && ny < canvas->height &&
                Canvas_GetPixel(canvas, nx, ny) == targetColorUint) {
                if (stackSize < maxStackSize - 1) {
                    stack[stackSize++] = (FloodFillPoint){nx, ny};
                }
            }
        }
    }
    
    free(stack);
}

typedef struct {
    int ymax;
    float x;
    float dx;
} EdgeTableEntry;

typedef struct {
    EdgeTableEntry* edges;
    int count;
    int capacity;
} EdgeList;

static EdgeList* CreateEdgeList(int capacity) {
    EdgeList* list = malloc(sizeof(EdgeList));
    if (!list) return NULL;
    
    list->edges = malloc(capacity * sizeof(EdgeTableEntry));
    if (!list->edges) {
        free(list);
        return NULL;
    }
    
    list->count = 0;
    list->capacity = capacity;
    return list;
}

static void DestroyEdgeList(EdgeList* list) {
    if (list) {
        free(list->edges);
        free(list);
    }
}

static void AddEdgeToList(EdgeList* list, int ymax, float x, float dx) {
    if (list->count < list->capacity) {
        list->edges[list->count].ymax = ymax;
        list->edges[list->count].x = x;
        list->edges[list->count].dx = dx;
        list->count++;
    }
}

static void SortEdgesByX(EdgeList* list) {
    for (int i = 0; i < list->count - 1; i++) {
        for (int j = 0; j < list->count - i - 1; j++) {
            if (list->edges[j].x > list->edges[j + 1].x) {
                EdgeTableEntry temp = list->edges[j];
                list->edges[j] = list->edges[j + 1];
                list->edges[j + 1] = temp;
            }
        }
    }
}

void Graphics_Fill_Scanline(Canvas* canvas, Polygon* poly, Color color) {
    if (!poly || poly->vertex_count < 3) return;
    
    int minY = poly->vertices[0].y;
    int maxY = poly->vertices[0].y;
    
    for (int i = 1; i < poly->vertex_count; i++) {
        if (poly->vertices[i].y < minY) minY = poly->vertices[i].y;
        if (poly->vertices[i].y > maxY) maxY = poly->vertices[i].y;
    }
    
    if (minY < 0) minY = 0;
    if (maxY >= canvas->height) maxY = canvas->height - 1;
    
    EdgeList** edgeTable = malloc((maxY - minY + 1) * sizeof(EdgeList*));
    for (int i = 0; i <= maxY - minY; i++) {
        edgeTable[i] = CreateEdgeList(poly->vertex_count);
    }
    
    for (int i = 0; i < poly->vertex_count; i++) {
        int j = (i + 1) % poly->vertex_count;
        int x1 = poly->vertices[i].x;
        int y1 = poly->vertices[i].y;
        int x2 = poly->vertices[j].x;
        int y2 = poly->vertices[j].y;
        
        if (y1 == y2) continue;
        
        if (y1 > y2) {
            int temp = x1; x1 = x2; x2 = temp;
            temp = y1; y1 = y2; y2 = temp;
        }
        
        if (y1 >= minY && y1 <= maxY) {
            float dx = (float)(x2 - x1) / (y2 - y1);
            AddEdgeToList(edgeTable[y1 - minY], y2, x1, dx);
        }
    }
    
    EdgeList* activeEdgeList = CreateEdgeList(poly->vertex_count);
    uint32_t fillColor = Graphics_ColorToUint32(color);
    
    for (int y = minY; y <= maxY; y++) {
        for (int i = 0; i < edgeTable[y - minY]->count; i++) {
            AddEdgeToList(activeEdgeList, 
                edgeTable[y - minY]->edges[i].ymax,
                edgeTable[y - minY]->edges[i].x,
                edgeTable[y - minY]->edges[i].dx);
        }
        
        for (int i = 0; i < activeEdgeList->count; i++) {
            if (activeEdgeList->edges[i].ymax <= y) {
                for (int j = i; j < activeEdgeList->count - 1; j++) {
                    activeEdgeList->edges[j] = activeEdgeList->edges[j + 1];
                }
                activeEdgeList->count--;
                i--;
            }
        }
        
        SortEdgesByX(activeEdgeList);
        
        for (int i = 0; i < activeEdgeList->count; i += 2) {
            if (i + 1 < activeEdgeList->count) {
                int xStart = (int)(activeEdgeList->edges[i].x + 0.5f);
                int xEnd = (int)(activeEdgeList->edges[i + 1].x + 0.5f);
                
                if (xStart < 0) xStart = 0;
                if (xEnd >= canvas->width) xEnd = canvas->width - 1;
                
                for (int x = xStart; x <= xEnd; x++) {
                    if (y >= 0 && y < canvas->height && x >= 0 && x < canvas->width) {
                        Canvas_SetPixel(canvas, x, y, fillColor);
                    }
                }
            }
        }
        
        for (int i = 0; i < activeEdgeList->count; i++) {
            activeEdgeList->edges[i].x += activeEdgeList->edges[i].dx;
        }
    }
    
    DestroyEdgeList(activeEdgeList);
    for (int i = 0; i <= maxY - minY; i++) {
        DestroyEdgeList(edgeTable[i]);
    }
    free(edgeTable);
}