#ifndef __MATHEMATICS_H__
#define __MATHEMATICS_H__

#include <math.h>


typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef float mat3[9];
typedef float mat4[16];
typedef struct {
	float w;
	float x;
	float y;
	float z;
} quat;


static inline void vec2Set(vec2 v, float x, float y)
{
	v[0] = x;
	v[1] = y;
}
static inline void vec2Copy(vec2 vs, vec2 vd)
{
	vd[0] = vs[0];
	vd[1] = vs[1];
}
static inline void vec2Add(vec2 va, vec2 vb, vec2 vr)
{
	vr[0] = va[0] + vb[0];
	vr[1] = va[1] + vb[1];
}
static inline void vec2Sub(vec2 va, vec2 vb, vec2 vr)
{
	vr[0] = va[0] - vb[0];
	vr[1] = va[1] - vb[1];
}
static inline void vec2Mul(vec2 va, float k, vec2 vr)
{
	vr[0] = va[0] * k;
	vr[1] = va[1] * k;
}
static inline void vec2Div(vec2 va, float k, vec2 vr)
{
	vr[0] = va[0] / k;
	vr[1] = va[1] / k;
}
float vec2Mag(vec2 v);
void vec2Normalize(vec2 v, vec2 vr);
float vec2Dot(vec2 va, vec2 vb);
void vec2Dump(vec2 v);

static inline void vec3Set(vec3 v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}
static inline void vec3Copy(vec3 vs, vec3 vd)
{
	vd[0] = vs[0];
	vd[1] = vs[1];
	vd[2] = vs[2];
}
static inline void vec3Add(vec3 va, vec3 vb, vec3 vr)
{
	vr[0] = va[0] + vb[0];
	vr[1] = va[1] + vb[1];
	vr[2] = va[2] + vb[2];
}
static inline void vec3Sub(vec3 va, vec3 vb, vec3 vr)
{
	vr[0] = va[0] - vb[0];
	vr[1] = va[1] - vb[1];
	vr[2] = va[2] - vb[2];
}
static inline void vec3Mul(vec3 va, float k, vec3 vr)
{
	vr[0] = va[0] * k;
	vr[1] = va[1] * k;
	vr[2] = va[2] * k;
}
static inline void vec3Div(vec3 va, float k, vec3 vr)
{
	vr[0] = va[0] / k;
	vr[1] = va[1] / k;
	vr[2] = va[2] / k;
}
float vec3Mag(vec3 v);
void vec3Normalize(vec3 v, vec3 vr);
float vec3Dot(vec3 va, vec3 vb);
void vec3Cross(vec3 va, vec3 vb, vec3 vr);
static inline void vec3FromPoints(vec3 v, float *a, float *b)
{
	v[0] = b[0] - a[0];
	v[1] = b[1] - a[1];
	v[2] = b[2] - a[2];
}
void vec3Normal(vec3 v, float *a, float *b, float *c);
void vec3Dump(vec3 v);

static inline void vec4Set(vec4 v, float x, float y, float z, float w)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}
static inline void vec4Copy(vec4 vs, vec4 vd)
{
	vd[0] = vs[0];
	vd[1] = vs[1];
	vd[2] = vs[2];
	vd[3] = vs[3];
}
static inline void vec4Add(vec4 va, vec4 vb, vec4 vr)
{
	vr[0] = va[0] + vb[0];
	vr[1] = va[1] + vb[1];
	vr[2] = va[2] + vb[2];
	vr[3] = va[3] + vb[3];
}
static inline void vec4Sub(vec4 va, vec4 vb, vec4 vr)
{
	vr[0] = va[0] - vb[0];
	vr[1] = va[1] - vb[1];
	vr[2] = va[2] - vb[2];
	vr[3] = va[3] - vb[3];
}
static inline void vec4Mul(vec4 va, float k, vec4 vr)
{
	vr[0] = va[0] * k;
	vr[1] = va[1] * k;
	vr[2] = va[2] * k;
	vr[3] = va[3] * k;
}
static inline void vec4Div(vec4 va, float k, vec4 vr)
{
	vr[0] = va[0] / k;
	vr[1] = va[1] / k;
	vr[2] = va[2] / k;
	vr[3] = va[3] / k;
}
float vec4Mag(vec4 v);
void vec4Normalize(vec4 v, vec4 vr);
float vec4Dot(vec4 va, vec4 vb);
void vec4Dump(vec4 v);

void mat3Identity(mat3 m);
void mat3Copy(mat3 ms, mat3 md);
void mat3Transpose(mat3 m, mat3 mr);
void mat3Add(mat3 ma, mat3 mb, mat3 mr);
void mat3Sub(mat3 ma, mat3 mb, mat3 mr);
void mat3Scale(mat3 m, float k, mat3 mr);
void mat3Mul(mat3 ma, mat3 mb, mat3 mr);
float mat3Det(mat3 m);
void mat3Inverse(mat3 m, mat3 mi);
void mat3InverseTranspose(mat3 m, mat3 mit);
void mat3Dump(mat3 m) ;

void mat4Identity(mat4 m);
void mat4Copy(mat4 ms, mat4 md);
void mat4To3(mat4 m4, mat3 m3);
void mat4Vec4Mul(mat4 m, vec4 v, vec4 r);
void mat4Orthographic(mat4 m, float l, float r, float t, 
	float b, float n, float f);
void mat4Frustum(mat4 m, float l, float r, float b, float t, float n, float f);
void mat4Perspective(mat4 m, float fovy, float aspect, float zn, float zf);
void mat4Mul(mat4 ma, mat4 mb, mat4 mr);
void mat4Translate(mat4 m, float x, float y, float z);
void mat4Scale(mat4 m, float x, float y, float z);
void mat4RotateX(mat4 m, float a);
void mat4RotateY(mat4 m, float a);
void mat4RotateZ(mat4 m, float a);
void mat4AnglesRotate(mat4 m, float ax, float ay, float az);
void mat4AxisRotate(mat4 m, vec3 axis, float angle);
void mat4Dump(mat4 m);

void quatConjugate(quat *q, quat *qr);
float quatMag(quat *q);
void quatNormalize(quat *q, quat *qr);
void quatMul(quat *qa, quat *qb, quat *qr);
void quatToMat4(quat *q, mat4 m);
;

#endif
