#ifndef UTILS_MATRIX_H
#define UTILS_MATRIX_H

#include "fixed.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    q16_16_t data[3][3];
} mat3x3_q16_16_t;

void mat3x3_identity(mat3x3_q16_16_t *m);
void mat3x3_mul_vec(const mat3x3_q16_16_t *m, const q16_16_t v[3], q16_16_t out[3]);
q16_16_t mat3x3_determinant(const mat3x3_q16_16_t *m);

#ifdef __cplusplus
}
#endif

#endif
