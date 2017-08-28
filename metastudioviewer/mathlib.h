#ifndef MATHLIB_H
#define MATHLIB_H

#define _USE_MATH_DEFINES
#include <math.h>

typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

typedef float mat4_t[4][4];

void VectorClear( vec3_t dest );
void VectorCopy( vec3_t src, vec3_t dest );
void VectorCopy4( vec4_t src, vec4_t dest );
void VectorMA( vec3_t vec1, float scale, vec3_t vec2, vec3_t dest );
void GL_PositionSlerp( vec3_t vec1, vec3_t vec2, float t, vec3_t dest );
void GL_QuaternionSlerp( vec4_t quat1, vec4_t quat2, float t, vec4_t dest );
void MatrixIdentity( mat4_t dest );
void MatrixCopy( mat4_t src, mat4_t dest );
void GL_QuaternionMatrix( vec4_t quat, mat4_t mat );
void GL_ConcatTransforms( mat4_t mat1, mat4_t mat2, mat4_t dest );
void GL_VectorRotate( vec3_t src, mat4_t mat, vec3_t dest );
void GL_VectorTransform( vec3_t src, mat4_t mat, vec3_t dest );

#endif