#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y, z, w;
} Vec4;

typedef struct {
    float m[4][4];
} Matrix4x4;

Matrix4x4 Matrix4x4_Identity();
Matrix4x4 Matrix4x4_Multiply(Matrix4x4 a, Matrix4x4 b);
Vec4 Matrix4x4_MultiplyVec4(Matrix4x4 m, Vec4 v);

Matrix4x4 Matrix4x4_Translation(float tx, float ty, float tz);
Matrix4x4 Matrix4x4_Scale(float sx, float sy, float sz);
Matrix4x4 Matrix4x4_RotationX(float angle);
Matrix4x4 Matrix4x4_RotationY(float angle);
Matrix4x4 Matrix4x4_RotationZ(float angle);

Vec4 Vec4_Create(float x, float y, float z, float w);
Vec3 Vec3_Create(float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif