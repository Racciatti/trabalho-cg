#include "canvas.h"
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

uint32_t Canvas_GetPixel(Canvas* canvas, int x, int y) {
    if (canvas && canvas->pixels && x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
        return canvas->pixels[y * canvas->width + x];
    }
    return 0;
}

int Canvas_LoadImage(Canvas* canvas, const char* filename) {
    if (!canvas || !filename) return 0;
    
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    
    if (!data) return 0;
    
    free(canvas->pixels);
    canvas->width = width;
    canvas->height = height;
    canvas->pixels = malloc(width * height * sizeof(uint32_t));
    
    if (!canvas->pixels) {
        stbi_image_free(data);
        return 0;
    }
    
    for (int i = 0; i < width * height; i++) {
        uint8_t r = data[i * 4 + 0];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];
        canvas->pixels[i] = (a << 24) | (b << 16) | (g << 8) | r;
    }
    
    stbi_image_free(data);
    return 1;
}