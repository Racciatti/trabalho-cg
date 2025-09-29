#include "canvas.h"
#include <stdlib.h>
#include <string.h>

Canvas* Canvas_Create(int width, int height) {
    Canvas* canvas = malloc(sizeof(Canvas));
    if (!canvas) return NULL;
    
    canvas->width = width;
    canvas->height = height;
    canvas->pixels = malloc(width * height * sizeof(uint32_t));
    if (!canvas->pixels) {
        free(canvas);
        return NULL;
    }
    
    Canvas_Clear(canvas, 0xFF000000);
    return canvas;
}

void Canvas_Destroy(Canvas* canvas) {
    if (canvas) {
        free(canvas->pixels);
        free(canvas);
    }
}

void Canvas_Clear(Canvas* canvas, uint32_t color) {
    if (canvas && canvas->pixels) {
        for (int i = 0; i < canvas->width * canvas->height; i++) {
            canvas->pixels[i] = color;
        }
    }
}

void Canvas_SetPixel(Canvas* canvas, int x, int y, uint32_t color) {
    if (canvas && canvas->pixels && x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
        canvas->pixels[y * canvas->width + x] = color;
    }
}