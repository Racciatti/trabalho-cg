#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "canvas.h"
#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CHANNEL_R,
    CHANNEL_G,
    CHANNEL_B
} Channel;

typedef struct {
    int a, b;
} Edge;

typedef struct {
    Vec3* vertices;
    int* edges; // Each edge is a pair of vertex indices: [a0, b0, a1, b1, ...]
    int vertex_count;
    int edge_count; // Number of edges (edges array has 2*edge_count ints)
} Object3D;

// Structures for Cohen-Sutherland clipping
typedef struct {
    int x1, y1;  // Start point
    int x2, y2;  // End point
} Line;

typedef struct {
    int xmin, ymin;  // Bottom-left corner
    int xmax, ymax;  // Top-right corner
} Rect;

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* vertices;
    int vertex_count;
    int capacity;
} Polygon;

void Graphics_ApplyGrayscale(Canvas* canvas);
void Graphics_ApplyNegative(Canvas* canvas);
void Graphics_ApplyChannel(Canvas* canvas, Channel c);

void Graphics_DrawLine_GeneralEquation(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color);
void Graphics_DrawLine_Parametric(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color);
void Graphics_DrawLine_Bresenham(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color);

void Graphics_DrawCircle_Explicit(Canvas* canvas, int xc, int yc, int r, uint32_t color);
void Graphics_DrawCircle_Parametric(Canvas* canvas, int xc, int yc, int r, uint32_t color);
void Graphics_DrawCircle_IncrementalRotation(Canvas* canvas, int xc, int yc, int r, uint32_t color);
void Graphics_DrawCircle_Bresenham(Canvas* canvas, int xc, int yc, int r, uint32_t color);

Object3D* Graphics_CreateCube();
void Graphics_DestroyObject3D(Object3D* object);
void Graphics_Translate3D(Object3D* object, float tx, float ty, float tz);
void Graphics_Scale3D(Object3D* object, float sx, float sy, float sz);
void Graphics_Rotate3D(Object3D* object, float angle, int axis);
void Graphics_RenderObject3D(Canvas* canvas, Object3D* object, Matrix4x4 transform, uint32_t color);

// Cohen-Sutherland line clipping
int Graphics_ClipLine_CohenSutherland(Line* line, Rect window);
void Graphics_DrawLine_Simple(Canvas* canvas, int x1, int y1, int x2, int y2, uint32_t color);
void Graphics_DrawRect(Canvas* canvas, Rect rect, uint32_t color);

Polygon* Graphics_CreatePolygon(int capacity);
void Graphics_DestroyPolygon(Polygon* poly);
void Graphics_AddVertexToPolygon(Polygon* poly, int x, int y);
void Graphics_DrawPolygon(Canvas* canvas, Polygon* poly, uint32_t color);
void Graphics_Fill_FloodFill(Canvas* canvas, int x, int y, Color targetColor, Color replacementColor, int connectivity);
void Graphics_Fill_Scanline(Canvas* canvas, Polygon* poly, Color color);
uint32_t Graphics_ColorToUint32(Color color);
Color Graphics_Uint32ToColor(uint32_t color);

#ifdef __cplusplus
}
#endif

#endif