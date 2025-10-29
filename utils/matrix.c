#include "matrix.h"

void mat3x3_identity(mat3x3_q16_16_t *m)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            m->data[i][j] = (i == j) ? Q16_16_ONE : 0;
        }
    }
}

void mat3x3_mul_vec(const mat3x3_q16_16_t *m, const q16_16_t v[3], q16_16_t out[3])
{
    for (int i = 0; i < 3; ++i) {
        q16_16_t acc = 0;
        for (int j = 0; j < 3; ++j) {
            acc += q16_16_mul(m->data[i][j], v[j]);
        }
        out[i] = acc;
    }
}

q16_16_t mat3x3_determinant(const mat3x3_q16_16_t *m)
{
    q16_16_t a = m->data[0][0];
    q16_16_t b = m->data[0][1];
    q16_16_t c = m->data[0][2];
    q16_16_t d = m->data[1][0];
    q16_16_t e = m->data[1][1];
    q16_16_t f = m->data[1][2];
    q16_16_t g = m->data[2][0];
    q16_16_t h = m->data[2][1];
    q16_16_t i = m->data[2][2];

    q16_16_t term1 = q16_16_mul(a, q16_16_mul(e, i) - q16_16_mul(f, h));
    q16_16_t term2 = q16_16_mul(b, q16_16_mul(d, i) - q16_16_mul(f, g));
    q16_16_t term3 = q16_16_mul(c, q16_16_mul(d, h) - q16_16_mul(e, g));

    return term1 - term2 + term3;
}
