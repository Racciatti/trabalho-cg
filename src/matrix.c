#include "matrix.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Matrix4x4 Matrix4x4_Identity() {
    Matrix4x4 result;
    memset(&result, 0, sizeof(Matrix4x4));
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

Matrix4x4 Matrix4x4_Multiply(Matrix4x4 a, Matrix4x4 b) {
    Matrix4x4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

Vec4 Matrix4x4_MultiplyVec4(Matrix4x4 m, Vec4 v) {
    Vec4 result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

Matrix4x4 Matrix4x4_Translation(float tx, float ty, float tz) {
    Matrix4x4 result = Matrix4x4_Identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;
    return result;
}

Matrix4x4 Matrix4x4_Scale(float sx, float sy, float sz) {
    Matrix4x4 result = Matrix4x4_Identity();
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = sz;
    return result;
}

Matrix4x4 Matrix4x4_RotationX(float angle) {
    Matrix4x4 result = Matrix4x4_Identity();
    float c = cos(angle);
    float s = sin(angle);
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    return result;
}

Matrix4x4 Matrix4x4_RotationY(float angle) {
    Matrix4x4 result = Matrix4x4_Identity();
    float c = cos(angle);
    float s = sin(angle);
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    return result;
}

Matrix4x4 Matrix4x4_RotationZ(float angle) {
    Matrix4x4 result = Matrix4x4_Identity();
    float c = cos(angle);
    float s = sin(angle);
    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;
    return result;
}

Vec4 Vec4_Create(float x, float y, float z, float w) {
    Vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

Vec3 Vec3_Create(float x, float y, float z) {
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}