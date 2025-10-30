/**
 * @file matrix3.c
 * @brief Determinant and inversion helpers for 3x3 matrices.
 */

#include "matrix3.h"

q16_16 matrix3_det(const matrix3_q16 *mat)
{
    const q16_16 a = mat->m[0][0];
    const q16_16 b = mat->m[0][1];
    const q16_16 c = mat->m[0][2];
    const q16_16 d = mat->m[1][0];
    const q16_16 e = mat->m[1][1];
    const q16_16 f = mat->m[1][2];
    const q16_16 g = mat->m[2][0];
    const q16_16 h = mat->m[2][1];
    const q16_16 i = mat->m[2][2];

    q16_16 term1 = q16_mul(a, q16_sub(q16_mul(e, i), q16_mul(f, h)));
    q16_16 term2 = q16_mul(b, q16_sub(q16_mul(d, i), q16_mul(f, g)));
    q16_16 term3 = q16_mul(c, q16_sub(q16_mul(d, h), q16_mul(e, g)));
    return q16_add(q16_sub(term1, term2), term3);
}

bool matrix3_inverse(const matrix3_q16 *mat, matrix3_q16 *inverse)
{
    q16_16 det = matrix3_det(mat);
    if (det == 0)
    {
        return false;
    }
    bool ok = true;
    q16_16 inv_det = q16_div(q16_from_int(1), det, &ok);
    if (!ok)
    {
        return false;
    }

    matrix3_q16 adj;
    adj.m[0][0] = q16_sub(q16_mul(mat->m[1][1], mat->m[2][2]), q16_mul(mat->m[1][2], mat->m[2][1]));
    adj.m[0][1] = q16_sub(q16_mul(mat->m[0][2], mat->m[2][1]), q16_mul(mat->m[0][1], mat->m[2][2]));
    adj.m[0][2] = q16_sub(q16_mul(mat->m[0][1], mat->m[1][2]), q16_mul(mat->m[0][2], mat->m[1][1]));

    adj.m[1][0] = q16_sub(q16_mul(mat->m[1][2], mat->m[2][0]), q16_mul(mat->m[1][0], mat->m[2][2]));
    adj.m[1][1] = q16_sub(q16_mul(mat->m[0][0], mat->m[2][2]), q16_mul(mat->m[0][2], mat->m[2][0]));
    adj.m[1][2] = q16_sub(q16_mul(mat->m[0][2], mat->m[1][0]), q16_mul(mat->m[0][0], mat->m[1][2]));

    adj.m[2][0] = q16_sub(q16_mul(mat->m[1][0], mat->m[2][1]), q16_mul(mat->m[1][1], mat->m[2][0]));
    adj.m[2][1] = q16_sub(q16_mul(mat->m[0][1], mat->m[2][0]), q16_mul(mat->m[0][0], mat->m[2][1]));
    adj.m[2][2] = q16_sub(q16_mul(mat->m[0][0], mat->m[1][1]), q16_mul(mat->m[0][1], mat->m[1][0]));

    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            inverse->m[r][c] = q16_mul(adj.m[r][c], inv_det);
        }
    }
    return true;
}
