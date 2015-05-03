#include <math.h>
#include <stdio.h>

#include "mathematics.h"

#define PI_DIV_180 0.01745329251994


/* vec2 */

float vec2Mag(vec2 v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1]);
}


void vec2Normalize(vec2 v, vec2 vr)
{
	float mag = vec2Mag(v);
	if(mag > 0.00001) {
		vr[0] = v[0] / mag;
		vr[1] = v[1] / mag;
	} else {
		vec2Copy(v, vr);
	}
}


float vec2Dot(vec2 va, vec2 vb)
{
	return va[0]*vb[0] + va[1]*vb[1];
}


void vec2Dump(vec2 v)
{
	printf("[%f, %f]\n", v[0], v[1]);
}


/* vec3 */

float vec3Mag(vec3 v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void vec3Normalize(vec3 v, vec3 vr)
{
	float mag = vec3Mag(v);
	if(mag > 0.00001) {
		vr[0] = v[0] / mag;
		vr[1] = v[1] / mag;
		vr[2] = v[2] / mag;
	} else {
		vec3Copy(v, vr);
	}
}


float vec3Dot(vec3 va, vec3 vb)
{
	return va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2];
}


void vec3Cross(vec3 va, vec3 vb, vec3 vr)
{
	vr[0] = va[1]*vb[2] - va[2]*vb[1];
	vr[1] = va[2]*vb[0] - va[0]*vb[2];
	vr[2] = va[0]*vb[1] - va[1]*vb[0];
}


void vec3Normal(vec3 v, float *a, float *b, float *c)
{
	vec3 vab, vac;

	vec3FromPoints(vab, a, b);
	vec3FromPoints(vac, a, c);
	vec3Cross(vab, vac, v);
	vec3Normalize(v, v);
}


void vec3Dump(vec3 v)
{
	printf("[%f, %f, %f]\n", v[0], v[1], v[2]);
}


/* vec4 */

float vec4Mag(vec4 v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}


void vec4Normalize(vec4 v, vec4 vr)
{
	float mag = vec4Mag(v);
	if(mag > 0.00001) {
		vr[0] = v[0] / mag;
		vr[1] = v[1] / mag;
		vr[2] = v[2] / mag;
		vr[3] = v[3] / mag;
	} else {
		vec4Copy(v, vr);
	}
}


float vec4Dot(vec4 va, vec4 vb)
{
	return va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2] + va[3]*vb[3];
}


void vec4Dump(vec4 v)
{
	printf("[%f, %f, %f, %f]\n", v[0], v[1], v[2], v[3]);
}


/* mat3 */

void mat3Identity(mat3 m)
{
	int i;
	for(i = 0; i < 9; i++) {
		m[i] = 0.0;
	}
	m[0] = 1.0;
	m[4] = 1.0;
	m[8] = 1.0;
}


void mat3Copy(mat3 ms, mat3 md)
{
	int i;
	for(i = 0; i < 9; i++) {
		md[i] = ms[i];
	}
}


void mat3Transpose(mat3 m, mat3 mr)
{
	int i;
	for(i = 0; i < 9; i++) {
		mr[8-i] = m[i];
	}
}


void mat3Add(mat3 ma, mat3 mb, mat3 mr)
{
	int i;
	for(i = 0; i < 9; i++) {
		mr[i] = ma[i] + mb[i];
	}
}


void mat3Sub(mat3 ma, mat3 mb, mat3 mr)
{
	int i;
	for(i = 0; i < 9; i++) {
		mr[i] = ma[i] - mb[i];
	}
}


void mat3Scale(mat3 m, float k, mat3 mr)
{
	int i;
	for(i = 0; i < 9; i++) {
		mr[i] = m[i] * k;
	}
}


void mat3Mul(mat3 ma, mat3 mb, mat3 mr)
{
	mr[0] = ma[0]*mb[0] + ma[1]*mb[3] + ma[2]*mb[6];	
	mr[1] = ma[0]*mb[1] + ma[1]*mb[4] + ma[2]*mb[7];	
	mr[2] = ma[0]*mb[2] + ma[1]*mb[5] + ma[2]*mb[8];	

	mr[3] = ma[3]*mb[0] + ma[4]*mb[3] + ma[5]*mb[6];	
	mr[4] = ma[3]*mb[1] + ma[4]*mb[4] + ma[5]*mb[7];	
	mr[5] = ma[3]*mb[2] + ma[4]*mb[5] + ma[5]*mb[8];	

	mr[6] = ma[6]*mb[0] + ma[7]*mb[3] + ma[8]*mb[6];	
	mr[7] = ma[6]*mb[1] + ma[7]*mb[4] + ma[8]*mb[7];	
	mr[8] = ma[6]*mb[2] + ma[7]*mb[5] + ma[8]*mb[8];	
}

/*
void mat3Mul(mat3 ma, mat3 mb, mat3 mr)
{
	int i, j, k;
	for(i = 0; i < 3; i++) {	
		for(j = 0; j < 3; j++) {
			mr[i+j*3] = 0.0;
			for(k = 0; k < 3; k++) {
				mr[i+j*3] += ma[i+k*3]*mb[k+j*3];
			}
		}
	}
} */


float mat3Det(mat3 m)
{
	float det = m[0] * (m[4]*m[8] - m[5]*m[7])
		- m[3] * (m[1]*m[8] - m[7]*m[2])
		+ m[6] * (m[1]*m[5] - m[2] * m[4]);
	return det;
}


void mat3Inverse(mat3 m, mat3 mi)
{
	float det = mat3Det(m);
	if(det > 0.0) {
		mi[0] = (m[4] * m[8] - m[5] * m[7]) / det;
		mi[1] = - (m[3] * m[8] - m[5]* m[6]) / det;
		mi[2] = (m[3] * m[7] - m[4] * m[6]) / det;

		mi[3] = - (m[1] * m[8] - m[2] * m[7]) / det;
		mi[4] = (m[0] * m[8] - m[2] * m[6]) / det;
		mi[5] = - (m[0] * m[7] - m[1] * m[6]) / det;

		mi[6] = (m[1] * m[5] - m[2] * m[4]) / det;
		mi[7] = - (m[0] * m[5] - m[2] * m[3]) / det;
		mi[8] = (m[0] * m[4] - m[1] * m[3]) / det;
	} else {
		mat3Identity(mi);
	}
}


void mat3InverseTranspose(mat3 m, mat3 mit)
{
	mat3 mi;
	mat3Inverse(m, mi);
	mat3Transpose(mi, mit);
}



void mat3Dump(mat3 m) 
{
	int i;
	printf("[");
	for(i=0; i<3; i++) 
		printf("%f %f %f\n", m[i*3], m[i*3+1], m[i*3+2]);
	printf("]\n");
}

/* mat4 */


void mat4Identity(mat4 m)
{
	int i;
	for(i = 0; i < 16; i++) {
		m[i] = 0.0;
	}
	m[0] = 1.0;
	m[5] = 1.0;
	m[10] = 1.0;
	m[15] = 1.0;
}


void mat4Copy(mat4 ms, mat4 md)
{
	int i;
	for(i = 0; i < 16; i++) {
		md[i] = ms[i];
	}
}


void mat4To3(mat4 m4, mat3 m3)
{
	int i;
	for(i = 0; i < 3; i++) {
		m3[i*3] = m4[i*4];
		m3[i*3+1] = m4[i*4+1];
		m3[i*3+2] = m4[i*4+2];
	}
}


void mat4Vec4Mul(mat4 m, vec4 v, vec4 r)
{
	int i;
	for(i=0; i<4; i++) {
		r[i] = m[i*4] * v[0] + m[i*4+1] * v[1]
			+ m[i*4+2] * v[2] + m[i*4+3] * v[3];
	}
}



void mat4Orthographic(mat4 m, float l, float r, float t, 
	float b, float n, float f) 
{
	int i;

	mat4 mtmp = {
		2.0f / (r-l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t-b), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (f-n), 0.0f,
		-(r+l) / (r-l), -(t+b) / (t-b), -(f+n) / (f-n), 1.0f
	};

	for(i = 0; i < 16; i++)
		m[i] = mtmp[i];
}


void mat4Frustum(mat4 m, float l, float r, float b, float t, float n, float f) 
{
	int i;

	float u = (2.0f * n) / (r - l);
	float v = (2.0f * n) / (t - b);
	float w = (r + l) / (r - l);
	float x = (t + b) / (t - b);
	float y = - (f + n) / (f - n);
	float z = - (2.0f * f * n) / (f - n);
	mat4 mtmp = {   u, 0.0f, 0.0f, 0.0f,
		0.0f,    v, 0.0f, 0.0f,
		   w,    x,    y,-1.0f,
		0.0f, 0.0f,    z, 0.0f};

	for(i = 0; i < 16; i++)
		m[i] = mtmp[i];
}


void mat4Perspective(mat4 m, float fovy, float aspect, float zn, float zf)
{
	float w;
	float h;

	h = tan(fovy / 2.0f * PI_DIV_180) * zn; 
	w = h * aspect;

	mat4Frustum(m, -w, w, -h, h, zn, zf);
}


void mat4Mul(mat4 ma, mat4 mb, mat4 mr)
{
	mr[0] = ma[0]*mb[0] + ma[1]*mb[4] + ma[2]*mb[8] + ma[3]*mb[12];
	mr[1] = ma[0]*mb[1] + ma[1]*mb[5] + ma[2]*mb[9] + ma[3]*mb[13];
	mr[2] = ma[0]*mb[2] + ma[1]*mb[6] + ma[2]*mb[10] + ma[3]*mb[14];
	mr[3] = ma[0]*mb[3] + ma[1]*mb[7] + ma[2]*mb[11] + ma[3]*mb[15];

	mr[4] = ma[4]*mb[0] + ma[5]*mb[4] + ma[6]*mb[8] + ma[7]*mb[12];
	mr[5] = ma[4]*mb[1] + ma[5]*mb[5] + ma[6]*mb[9] + ma[7]*mb[13];
	mr[6] = ma[4]*mb[2] + ma[5]*mb[6] + ma[6]*mb[10] + ma[7]*mb[14];
	mr[7] = ma[4]*mb[3] + ma[5]*mb[7] + ma[6]*mb[11] + ma[7]*mb[15];

	mr[8] = ma[8]*mb[0] + ma[9]*mb[4] + ma[10]*mb[8] + ma[11]*mb[12];
	mr[9] = ma[8]*mb[1] + ma[9]*mb[5] + ma[10]*mb[9] + ma[11]*mb[13];
	mr[10] = ma[8]*mb[2] + ma[9]*mb[6] + ma[10]*mb[10] + ma[11]*mb[14];
	mr[11] = ma[8]*mb[3] + ma[9]*mb[7] + ma[10]*mb[11] + ma[11]*mb[15];

	mr[12] = ma[12]*mb[0] + ma[13]*mb[4] + ma[14]*mb[8] + ma[15]*mb[12];
	mr[13] = ma[12]*mb[1] + ma[13]*mb[5] + ma[14]*mb[9] + ma[15]*mb[13];
	mr[14] = ma[12]*mb[2] + ma[13]*mb[6] + ma[14]*mb[10] + ma[15]*mb[14];
	mr[15] = ma[12]*mb[3] + ma[13]*mb[7] + ma[14]*mb[11] + ma[15]*mb[15];
}

/*
void mat4Mul(mat4 ma, mat4 mb, mat4 mr)
{
	int i, j, k;
	for(i = 0; i < 4; i++) {	
		for(j = 0; j < 4; j++) {
			mr[i+j*4] = 0.0;
			for(k = 0; k < 4; k++) {
				mr[i+j*4] += ma[i+k*4]*mb[k+j*4];
			}
		}
	}
} */


void mat4Translate(mat4 m, float x, float y, float z) 
{
	mat4Identity(m);

	m[12] = x;
	m[13] = y;
	m[14] = z;
}


void mat4Scale(mat4 m, float x, float y, float z) 
{
	mat4Identity(m);
	
	m[0] = x;
	m[5] = y;
	m[10] = z;
}


void mat4RotateX(mat4 m, float a) 
{
	float cosa = cos(a);
	float sina = sin(a);
	mat4Identity(m);

	m[5] = cosa;
	m[6] = -sina;
	m[9] = sina;
	m[10] = cosa;
}


void mat4RotateY(mat4 m, float a) 
{
	float cosa = cos(a);
	float sina = sin(a);
	mat4Identity(m);

	m[0] = cosa;
	m[2] = sina;
	m[8] = -sina;
	m[10] = cosa;
}


void mat4RotateZ(mat4 m, float a) 
{
	float cosa = cos(a);
	float sina = sin(a);
	mat4Identity(m);

	m[0] = cosa;
	m[1] = sina;
	m[4] = -sina;
	m[5] = cosa;
}


void mat4AnglesRotate(mat4 m, float ax, float ay, float az)
{
	mat4 rx, ry, rz;
	mat4 tmp;

	mat4RotateX(rx, ax);
	mat4RotateY(ry, ay);
	mat4RotateZ(rz, az);
	mat4Mul(rx, ry, tmp);

	mat4Mul(tmp, rz, m);
}


void mat4AxisRotate(mat4 m, vec3 axis, float angle)
{
	float sina = sin(angle / 2); 
	float cosa = cos(angle / 2);
	quat q, qn;

	q.x = axis[0] * sina;
	q.y = axis[1] * sina;
	q.z = axis[2] * sina;
	q.w = cosa;
	quatNormalize(&q, &qn);
	quatToMat4(&q, m);
}


void mat4Dump(mat4 m) 
{
	int i;
	printf("[");
	for(i=0; i<4; i++) 
		printf("%f %f %f %f\n", m[i*4], m[i*4+1], m[i*4+2], m[i*4+3]);
	printf("]\n");
}

/* quaternion */


void quatConjugate(quat *q, quat *qr)
{
	qr->w = q->w;
	qr->x = -q->x;
	qr->y = -q->y;
	qr->z = -q->z;
}


float quatMag(quat *q)
{
	return sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
}


void quatNormalize(quat *q, quat *qr)
{
	float mag = quatMag(q);

	if(mag > 0.0) {
		qr->w = q->w / mag;
		qr->x = q->x / mag;
		qr->y = q->y / mag;
		qr->z = q->z / mag;
	} else {
		qr->w = q->w;
		qr->x = q->x;
		qr->y = q->y;
		qr->z = q->z;
	}
}


void quatMul(quat *qa, quat *qb, quat *qr)
{
	qr->w = qa->w*qb->w - qa->x*qb->x - qa->y*qb->y - qa->z*qb->z;
	qr->x = qa->w*qb->x - qa->x*qb->w + qa->y*qb->z - qa->z*qb->y;
	qr->y = qa->w*qb->y - qa->x*qb->z + qa->y*qb->w + qa->z*qb->x;
	qr->z = qa->w*qb->z + qa->x*qb->y - qa->y*qb->x + qa->z*qb->w;
}


void quatToMat4(quat *q, mat4 m)
{
	float xx = q->x * q->x;
	float xy = q->x * q->y;
	float xz = q->x * q->z;
	float xw = q->x * q->w;
	float yy = q->y * q->y;	
	float yz = q->y * q->z;
	float yw = q->y * q->w;
	float zz = q->z * q->z;
	float zw = q->z * q->w;

	m[0] = 1 - 2 * (yy+zz);
	m[1] = 2 * (xy-zw);
	m[2] = 2 * (xz+yw);
	m[3] = 0.0;
	m[4] = 2 * (xy+zw);
	m[5] = 1 - 2 * (xx+zz);
	m[6] = 2 * (yz-xw);
	m[7] = 0.0;
	m[8] = 2 * (xz-yw);
	m[9] = 2 * (yz+xw);
	m[10] = 1 - 2 * (xx+yy);
	m[11] = 0.0;
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
}


