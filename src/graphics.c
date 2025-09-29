#include "graphics.h"
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