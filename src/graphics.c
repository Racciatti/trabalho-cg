#include "graphics.h"
#include <stdint.h>

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