#ifndef UI_H
#define UI_H

#include "canvas.h"
#include "graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

void UI_Render(void);
void UI_SetCanvas(Canvas* canvas);
void UI_SetPixelInfo(const char* info);
bool UI_TextureNeedsUpdate(void);
int UI_GetSelectedLineAlgorithm(void);
int UI_GetSelectedCircleAlgorithm(void);
void UI_TriggerTextureUpdate(void);
void UI_Cleanup(void);
void UI_StartClippingMode(void);
void UI_StartPolygonDrawing(void);
int UI_GetSelectedFillAlgorithm(void);
void UI_FillPolygonScanline(void);
void UI_SetCurrentPolygon(Polygon* poly);

#ifdef __cplusplus
}
#endif

#endif