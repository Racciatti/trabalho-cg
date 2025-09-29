#include "graphics.h"
#include "matrix.h"
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

Object3D* Graphics_CreateCube() {
    Object3D* cube = malloc(sizeof(Object3D));
    if (!cube) return NULL;
    
    cube->vertex_count = 8;
    cube->vertices = malloc(8 * sizeof(Vec3));
    if (!cube->vertices) {
        free(cube);
        return NULL;
    }
    
    cube->vertices[0] = Vec3_Create(-50, -50, -50);
    cube->vertices[1] = Vec3_Create( 50, -50, -50);
    cube->vertices[2] = Vec3_Create( 50,  50, -50);
    cube->vertices[3] = Vec3_Create(-50,  50, -50);
    cube->vertices[4] = Vec3_Create(-50, -50,  50);
    cube->vertices[5] = Vec3_Create( 50, -50,  50);
    cube->vertices[6] = Vec3_Create( 50,  50,  50);
    cube->vertices[7] = Vec3_Create(-50,  50,  50);
    
    cube->edge_count = 12;
    cube->edges = malloc(12 * sizeof(Edge));
    if (!cube->edges) {
        free(cube->vertices);
        free(cube);
        return NULL;
    }
    
    cube->edges[0] = (Edge){0, 1};
    cube->edges[1] = (Edge){1, 2};
    cube->edges[2] = (Edge){2, 3};
    cube->edges[3] = (Edge){3, 0};
    cube->edges[4] = (Edge){4, 5};
    cube->edges[5] = (Edge){5, 6};
    cube->edges[6] = (Edge){6, 7};
    cube->edges[7] = (Edge){7, 4};
    cube->edges[8] = (Edge){0, 4};
    cube->edges[9] = (Edge){1, 5};
    cube->edges[10] = (Edge){2, 6};
    cube->edges[11] = (Edge){3, 7};
    
    return cube;
}

void Graphics_DestroyObject3D(Object3D* object) {
    if (object) {
        free(object->vertices);
        free(object->edges);
        free(object);
    }
}

void Graphics_Translate3D(Object3D* object, float tx, float ty, float tz) {
    if (!object) return;
    
    Matrix4x4 translation = Matrix4x4_Translation(tx, ty, tz);
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(translation, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_Scale3D(Object3D* object, float sx, float sy, float sz) {
    if (!object) return;
    
    Vec3 center = Vec3_Create(0, 0, 0);
    for (int i = 0; i < object->vertex_count; i++) {
        center.x += object->vertices[i].x;
        center.y += object->vertices[i].y;
        center.z += object->vertices[i].z;
    }
    center.x /= object->vertex_count;
    center.y /= object->vertex_count;
    center.z /= object->vertex_count;
    
    Matrix4x4 toOrigin = Matrix4x4_Translation(-center.x, -center.y, -center.z);
    Matrix4x4 scale = Matrix4x4_Scale(sx, sy, sz);
    Matrix4x4 fromOrigin = Matrix4x4_Translation(center.x, center.y, center.z);
    Matrix4x4 transform = Matrix4x4_Multiply(fromOrigin, Matrix4x4_Multiply(scale, toOrigin));
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(transform, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_Rotate3D(Object3D* object, float angle, int axis) {
    if (!object) return;
    
    Vec3 center = Vec3_Create(0, 0, 0);
    for (int i = 0; i < object->vertex_count; i++) {
        center.x += object->vertices[i].x;
        center.y += object->vertices[i].y;
        center.z += object->vertices[i].z;
    }
    center.x /= object->vertex_count;
    center.y /= object->vertex_count;
    center.z /= object->vertex_count;
    
    Matrix4x4 toOrigin = Matrix4x4_Translation(-center.x, -center.y, -center.z);
    Matrix4x4 rotation;
    switch (axis) {
        case 0: rotation = Matrix4x4_RotationX(angle); break;
        case 1: rotation = Matrix4x4_RotationY(angle); break;
        case 2: rotation = Matrix4x4_RotationZ(angle); break;
        default: rotation = Matrix4x4_Identity(); break;
    }
    Matrix4x4 fromOrigin = Matrix4x4_Translation(center.x, center.y, center.z);
    Matrix4x4 transform = Matrix4x4_Multiply(fromOrigin, Matrix4x4_Multiply(rotation, toOrigin));
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        Vec4 transformed = Matrix4x4_MultiplyVec4(transform, v);
        object->vertices[i].x = transformed.x;
        object->vertices[i].y = transformed.y;
        object->vertices[i].z = transformed.z;
    }
}

void Graphics_RenderObject3D(Canvas* canvas, Object3D* object, Matrix4x4 transform, uint32_t color) {
    if (!canvas || !object) return;
    
    Vec4* transformed_vertices = malloc(object->vertex_count * sizeof(Vec4));
    if (!transformed_vertices) return;
    
    for (int i = 0; i < object->vertex_count; i++) {
        Vec4 v = Vec4_Create(object->vertices[i].x, object->vertices[i].y, object->vertices[i].z, 1.0f);
        transformed_vertices[i] = Matrix4x4_MultiplyVec4(transform, v);
    }
    
    int centerX = canvas->width / 2;
    int centerY = canvas->height / 2;
    
    for (int i = 0; i < object->edge_count; i++) {
        Edge edge = object->edges[i];
        Vec4 v1 = transformed_vertices[edge.a];
        Vec4 v2 = transformed_vertices[edge.b];
        
        int x1 = centerX + (int)v1.x;
        int y1 = centerY + (int)v1.y;
        int x2 = centerX + (int)v2.x;
        int y2 = centerY + (int)v2.y;
        
        Graphics_DrawLine_Bresenham(canvas, x1, y1, x2, y2, color);
    }
    
    free(transformed_vertices);
}