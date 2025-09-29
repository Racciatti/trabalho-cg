#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int width;
    int height;
    uint32_t* pixels;
} Canvas;

Canvas* Canvas_Create(int width, int height);
void Canvas_Destroy(Canvas* canvas);
void Canvas_Clear(Canvas* canvas, uint32_t color);
void Canvas_SetPixel(Canvas* canvas, int x, int y, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif