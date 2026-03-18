#ifndef LENS_MATH_H
#define LENS_MATH_H

#include <math.h>

// 1D array-backed 3x3 affine matrix for transform math
// Indices:
// 0: m00, 1: m01, 2: tx
// 3: m10, 4: m11, 5: ty
// 6: 0,   7: 0,   8: 1
typedef struct {
    float m[9];
} LensMatrix3x3;

static inline LensMatrix3x3 lens_matrix_identity(void) {
    LensMatrix3x3 mat = {
        .m = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        }
    };
    return mat;
}

static inline LensMatrix3x3 lens_matrix_multiply(LensMatrix3x3 a, LensMatrix3x3 b) {
    LensMatrix3x3 result;
    // Row 0
    result.m[0] = a.m[0] * b.m[0] + a.m[1] * b.m[3] + a.m[2] * b.m[6];
    result.m[1] = a.m[0] * b.m[1] + a.m[1] * b.m[4] + a.m[2] * b.m[7];
    result.m[2] = a.m[0] * b.m[2] + a.m[1] * b.m[5] + a.m[2] * b.m[8];
    // Row 1
    result.m[3] = a.m[3] * b.m[0] + a.m[4] * b.m[3] + a.m[5] * b.m[6];
    result.m[4] = a.m[3] * b.m[1] + a.m[4] * b.m[4] + a.m[5] * b.m[7];
    result.m[5] = a.m[3] * b.m[2] + a.m[4] * b.m[5] + a.m[5] * b.m[8];
    // Row 2
    result.m[6] = a.m[6] * b.m[0] + a.m[7] * b.m[3] + a.m[8] * b.m[6];
    result.m[7] = a.m[6] * b.m[1] + a.m[7] * b.m[4] + a.m[8] * b.m[7];
    result.m[8] = a.m[6] * b.m[2] + a.m[7] * b.m[5] + a.m[8] * b.m[8];
    return result;
}

static inline LensMatrix3x3 lens_matrix_translation(float tx, float ty) {
    LensMatrix3x3 mat = lens_matrix_identity();
    mat.m[2] = tx;
    mat.m[5] = ty;
    return mat;
}

static inline LensMatrix3x3 lens_matrix_scale(float sx, float sy) {
    LensMatrix3x3 mat = lens_matrix_identity();
    mat.m[0] = sx;
    mat.m[4] = sy;
    return mat;
}

static inline LensMatrix3x3 lens_matrix_rotation(float radians) {
    LensMatrix3x3 mat = lens_matrix_identity();
    float c = cosf(radians);
    float s = sinf(radians);
    mat.m[0] = c;  mat.m[1] = -s;
    mat.m[3] = s;  mat.m[4] = c;
    return mat;
}

#endif // LENS_MATH_H
