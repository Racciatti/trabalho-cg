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
    int vertex_count;
    Edge* edges;
    int edge_count;
} Object3D;

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

#ifdef __cplusplus
}
#endif

#endif