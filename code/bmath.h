#if !defined(BMATH_H)
#define BMATH_H

#include <immintrin.h>

#include <math.h>

#define PI32 3.14159265359f

//Convert degrees to radians in 32bit format
BATCH_INLINE const f32
to_radians_32(const f32 deg) { return deg * PI32 / 180.0f; }

typedef f32 vec2[2];
typedef f32 vec3[3];
typedef f32 vec4[4];

typedef f32 mat3[3][3];
typedef f32 mat4[4][4];

#define MAT3_IDENTITY  {{1.0f, 0.0f, 0.0f},                          \
                            {0.0f, 1.0f, 0.0f},                          \
                            {0.0f, 0.0f, 1.0f}}

#define MAT4_IDENTITY {{1.0f, 0.0f, 0.0f, 0.0f},                    \
                       {0.0f, 1.0f, 0.0f, 0.0f},                    \
                       {0.0f, 0.0f, 1.0f, 0.0f},                    \
                       {0.0f, 0.0f, 0.0f, 1.0f}}

/*Vec2 Area*/
BATCH_INLINE void
vec2_copy(const vec2 a, vec2 dest) {
	dest[0] = a[0];
	dest[1] = a[1];
}

/*Vec3 Area*/

BATCH_INLINE void
vec3_copy(const vec3 a, vec3 dest) {
	dest[0] = a[0];
	dest[1] = a[1];
	dest[2] = a[2];
}

BATCH_INLINE void
vec3_mul(const vec3 a, const vec3 b, vec3 dest) {
	dest[0] = a[0] * b[0];
	dest[1] = a[1] * b[1];
	dest[2] = a[2] * b[2];
}

BATCH_INLINE f32
vec3_dot(const vec3 a, const vec3 b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

BATCH_INLINE f32
vec3_norm2(const vec3 v) {
	return vec3_dot(v, v);
}

BATCH_INLINE f32
vec3_norm(const vec3 v) {
	return sqrtf(vec3_norm2(v));
}

BATCH_INLINE void
vec3_scale(const vec3 v, const f32 s, vec3 dest) {
	dest[0] = v[0] * s;
	dest[1] = v[1] * s;
	dest[2] = v[2] * s;
}

BATCH_INLINE void
vec3_add(const vec3 v, vec3 dest) {
	dest[0] += v[0];
	dest[1] += v[1];
	dest[2] += v[2];
}

BATCH_INLINE void
vec3_sub(const vec3 v, vec3 dest) {
	dest[0] -= v[0];
	dest[1] -= v[1];
	dest[2] -= v[2];
}

BATCH_INLINE void
vec3_normalize(vec3 v) {
	const f32 norm = vec3_norm(v);

	if(norm == 0.0f) {
		v[0] = v[1] = v[2] = 0.0f;
		return;
	}

	vec3_scale(v, 1.0f / norm, v);
}

BATCH_INLINE void
vec3_cross(const vec3 a, const vec3 b, vec3 dest) {
	vec3 c;
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
	vec3_copy(c, dest);
}

BATCH_INLINE void
vec3_normalize_to(const vec3 v, vec3 dest) {
	const f32 norm = vec3_norm(v);

	if(norm == 0.0f) {
		dest[0] = dest[1] = dest[2] = 0.0f;
		return;
	}

	vec3_scale(v, 1.0f / norm, dest);
}

BATCH_INLINE void
vec3_crossn(const vec3 a, const vec3 b, vec3 dest) {
	vec3_cross(a, b, dest);
	vec3_normalize(dest);
}

/*Vec4 Area*/

BATCH_INLINE void
vec4_copy(const vec4 v, vec4 dest) {
	dest[0] = v[0];
	dest[1] = v[1];
	dest[2] = v[2];
	dest[3] = v[3];
}

BATCH_INLINE void
vec4_muladds(const vec4 a, const f32 s, vec4 dest) {
	dest[0] += a[0] * s;
	dest[1] += a[1] * s;
	dest[2] += a[2] * s;
	dest[3] += a[3] * s;
}

BATCH_INLINE void
vec4_scale(const vec4 v, const f32 s, vec4 dest) {
	dest[0] = v[0] * s;
	dest[1] = v[1] * s;
	dest[2] = v[2] * s;
	dest[3] = v[3] * s;
}

/*Mat3 Area*/

BATCH_INLINE void
mat3_translate(mat3 m, const vec2 v) {
	m[2][0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0];
	m[2][1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1];
	m[2][2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2];
}

BATCH_INLINE void
mat3_rotate(mat3 m, const f32 angle) {
	const f32 m00 = m[0][0],  m10 = m[1][0],
	          m01 = m[0][1],  m11 = m[1][1],
	          m02 = m[0][2],  m12 = m[1][2];
	const f32 s = sinf(angle), c = cosf(angle);
	
	m[0][0] = m00 * c + m10 * s;
	m[0][1] = m01 * c + m11 * s;
	m[0][2] = m02 * c + m12 * s;
	
	m[1][0] = m00 * -s + m10 * c;
	m[1][1] = m01 * -s + m11 * c;
	m[1][2] = m02 * -s + m12 * c;
}

BATCH_INLINE void
mat3_scale(mat3 m, const vec2 v) {
	m[0][0] = m[0][0] * v[0];
	m[0][1] = m[0][1] * v[0];
	m[0][2] = m[0][2] * v[0];
	
	m[1][0] = m[1][0] * v[1];
	m[1][1] = m[1][1] * v[1];
	m[1][2] = m[1][2] * v[1];
}

/*Mat4 Area*/

BATCH_INLINE void
mat4_identity(mat4 dest) {
	dest[0][0] = 1.0f; dest[0][1] = 0.0f; dest[0][2] = 0.0f; dest[0][3] = 0.0f;
	dest[1][0] = 0.0f; dest[1][1] = 1.0f; dest[1][2] = 0.0f; dest[1][3] = 0.0f;
	dest[2][0] = 0.0f; dest[2][1] = 0.0f; dest[2][2] = 1.0f; dest[2][3] = 0.0f;
	dest[3][0] = 0.0f; dest[3][1] = 0.0f; dest[3][2] = 0.0f; dest[3][3] = 1.0f;
}

BATCH_INLINE void
mat4_copy(const mat4 mat, mat4 dest) {
	dest[0][0] = mat[0][0];  dest[1][0] = mat[1][0];
	dest[0][1] = mat[0][1];  dest[1][1] = mat[1][1];
	dest[0][2] = mat[0][2];  dest[1][2] = mat[1][2];
	dest[0][3] = mat[0][3];  dest[1][3] = mat[1][3];
	
	dest[2][0] = mat[2][0];  dest[3][0] = mat[3][0];
	dest[2][1] = mat[2][1];  dest[3][1] = mat[3][1];
	dest[2][2] = mat[2][2];  dest[3][2] = mat[3][2];
	dest[2][3] = mat[2][3];  dest[3][3] = mat[3][3];
}

BATCH_INLINE __m128
mm_fmadd(__m128 a, __m128 b, __m128 c) {
  return _mm_add_ps(c, _mm_mul_ps(a, b));
}

BATCH_INLINE void
mat4_mulv4(const mat4 m, const vec4 v, vec4 dest) {
	#if 0
	vec4 res;
	res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
	vec4_copy(res, dest);
	#else

	__m128 x0, x1, m0, m1, m2, m3, v0, v1, v2, v3;

	m0 = _mm_load_ps(m[0]);
	m1 = _mm_load_ps(m[1]);
	m2 = _mm_load_ps(m[2]);
	m3 = _mm_load_ps(m[3]);

	x0 = _mm_load_ps(v);

	v0 = _mm_set_ps1(v[0]);
	v1 = _mm_set_ps1(v[1]);
	v2 = _mm_set_ps1(v[2]);
	v3 = _mm_set_ps1(v[3]);

	x1 = _mm_mul_ps(m3, v3);
	x1 = mm_fmadd(m2, v2, x1);
	x1 = mm_fmadd(m1, v1, x1);
	x1 = mm_fmadd(m0, v0, x1);

	_mm_store_ps(dest, x1);

	#endif
}

BATCH_INLINE void
mat4_mulv3(const mat4 m, const f32 last, const vec3 v, vec3 dest) {
	#if 1
	vec4 res;
	res[0] = v[0];
	res[1] = v[1];
	res[2] = v[2];
	res[3] = last;

	mat4_mulv4(m, res, res);
	dest[0] = res[0];
	dest[1] = res[1];
	dest[2] = res[2];
	#else
	vec3 res;
	res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * last;
	res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * last;
	res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * last;
	vec3_copy(res, dest);
	
	#endif
}

BATCH_INLINE void
mat4_zero(mat4 dest) {
	dest[0][0] = 0.0f; dest[0][1] = 0.0f; dest[0][2] = 0.0f; dest[0][3] = 0.0f;
	dest[1][0] = 0.0f; dest[1][1] = 0.0f; dest[1][2] = 0.0f; dest[1][3] = 0.0f;
	dest[2][0] = 0.0f; dest[2][1] = 0.0f; dest[2][2] = 0.0f; dest[2][3] = 0.0f;
	dest[3][0] = 0.0f; dest[3][1] = 0.0f; dest[3][2] = 0.0f; dest[3][3] = 0.0f;
}

BATCH_INLINE void
mat4_translate(mat4 dest, const vec3 v) {
	vec4_muladds(dest[0], v[0], dest[3]);
	vec4_muladds(dest[1], v[1], dest[3]);
	vec4_muladds(dest[2], v[2], dest[3]);
}

BATCH_INLINE void
mat4_rotate_make(mat4 m, const f32 angle, const vec3 axis) {
	vec3 axisn, v, vs;
	
	const f32 c = cosf(angle);
	
	vec3_normalize_to(axis, axisn);
	vec3_scale(axisn, 1.0f - c, v);
	vec3_scale(axisn, sinf(angle), vs);
	
	vec3_scale(axisn, v[0], m[0]);
	vec3_scale(axisn, v[1], m[1]);
	vec3_scale(axisn, v[2], m[2]);
	
	m[0][0] += c;       m[1][0] -= vs[2];   m[2][0] += vs[1];
	m[0][1] += vs[2];   m[1][1] += c;       m[2][1] -= vs[0];
	m[0][2] -= vs[1];   m[1][2] += vs[0];   m[2][2] += c;
	
	m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

BATCH_INLINE void
mat4_mul(const mat4 m1, const mat4 m2, mat4 dest) {
	const f32 a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
	          a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
	          a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
	          a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],
	
	          b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
	          b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
	          b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
	          b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];
	
	dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
	dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
	dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
	dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
	dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
	dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
	dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
	dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
	dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
	dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
	dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
	dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
	dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
	dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
	dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
	dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

BATCH_INLINE void
mat4_mul_rot(const mat4 m1, const mat4 m2, mat4 dest) {
	const f32 a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
              a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
              a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
              a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],
  
              b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2],
              b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2],
              b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2];

 	dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02;
 	dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02;
 	dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02;
 	dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02;

 	dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12;
 	dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12;
 	dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12;
 	dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12;

 	dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22;
 	dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22;
 	dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22;
 	dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22;

 	dest[3][0] = a30;
 	dest[3][1] = a31;
 	dest[3][2] = a32;
 	dest[3][3] = a33;
}

BATCH_INLINE void
mat4_rotate(mat4 m, const f32 angle, const vec3 axis) {
	mat4 rot;
	mat4_rotate_make(rot, angle, axis);
	mat4_mul_rot(m, rot, m);
}

BATCH_INLINE void
mat4_scale_to(mat4 m, const vec3 v, mat4 dest) {
	vec4_scale(m[0], v[0], dest[0]);
	vec4_scale(m[1], v[1], dest[1]);
	vec4_scale(m[2], v[2], dest[2]);
	
	vec4_copy(m[3], dest[3]);
}

BATCH_INLINE void
mat4_scale(mat4 m, const vec3 v) {
	mat4_scale_to(m, v, m);
}

BATCH_INLINE void
mat4_ortho(const f32 left,   const f32 right,
           const f32 bottom, const f32 top,
           const f32 nearZ,  const f32 farZ,
           mat4  dest) {
  const f32 rl = 1.0f / (right  - left),
		    tb = 1.0f / (top    - bottom),
            fn =-1.0f / (farZ   - nearZ);

  mat4_zero(dest);

  dest[0][0] = 2.0f * rl;
  dest[1][1] = 2.0f * tb;
  dest[2][2] =-fn;
  dest[3][0] =-(right  + left)    * rl;
  dest[3][1] =-(top    + bottom)  * tb;
  dest[3][2] = nearZ * fn;
  dest[3][3] = 1.0f;
}

BATCH_INLINE void
mat4_perspective(f32 fov, f32 aspect, f32 near, f32 far, mat4 dest) {
	f32 f, fn;

	mat4_zero(dest);
	
	f  = 1.0f / tanf(fov * 0.5f);
	fn = 1.0f / (near - far);
	
	dest[0][0] = f / aspect;
	dest[1][1] = f;
	dest[2][2] =-far * fn;
	dest[2][3] = 1.0f;
	dest[3][2] = near * far * fn;
}

BATCH_INLINE void
mat4_look_at(const vec3 eye, const vec3 center, const vec3 up, mat4 dest) {
	vec3 f, u, s;

	vec3_copy(center, f);
	vec3_sub(eye, f);
	vec3_normalize(f);
	
	vec3_crossn(up, f, s);
	vec3_cross(f, s, u);
	
	dest[0][0] = s[0];
	dest[0][1] = u[0];
	dest[0][2] = f[0];
	dest[1][0] = s[1];
	dest[1][1] = u[1];
	dest[1][2] = f[1];
	dest[2][0] = s[2];
	dest[2][1] = u[2];
	dest[2][2] = f[2];
	dest[3][0] =-vec3_dot(s, eye);
	dest[3][1] =-vec3_dot(u, eye);
	dest[3][2] =-vec3_dot(f, eye);
	dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
	dest[3][3] = 1.0f;
}

#endif
