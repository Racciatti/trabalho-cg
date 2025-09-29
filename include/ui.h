#ifndef UI_H
#define UI_H

#include "canvas.h"

#ifdef __cplusplus
extern "C" {
#endif

void UI_Render(void);
void UI_SetCanvas(Canvas* canvas);
void UI_SetPixelInfo(const char* info);
bool UI_TextureNeedsUpdate(void);

#ifdef __cplusplus
}
#endif

#endif