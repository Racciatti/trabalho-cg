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

#ifdef __cplusplus
}
#endif

#endif