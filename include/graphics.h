#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "canvas.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CHANNEL_R,
    CHANNEL_G,
    CHANNEL_B
} Channel;

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

#ifdef __cplusplus
}
#endif

#endif