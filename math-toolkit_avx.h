#ifndef __RAY_MATH_TOOLKIT_H
#define __RAY_MATH_TOOLKIT_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <immintrin.h>
__attribute__((always_inline)) static inline
void normalize(double *v)
{
    double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    assert(d != 0.0 && "Error calculating normal");

    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

__attribute__((always_inline)) static inline
double length(const double *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

__attribute__((always_inline)) static inline
void add_vector(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] + b[i];
}

__attribute__((always_inline)) static inline
void subtract_vector(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] - b[i];
}

__attribute__((always_inline)) static inline
void multiply_vectors(const double *a, const double *b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] * b[i];
}

__attribute__((always_inline)) static inline
void multiply_vector(const double *a, double b, double *out)
{
    for (int i = 0; i < 3; i++)
        out[i] = a[i] * b;
}

__attribute__((always_inline)) static inline
void cross_product(const double *v1, const double *v2, double *out)
{
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static inline
double dot_product(const double *v1, const double *v2)
{
    double temp[4] = {0, 0, 0, 0};
    __m256d ymm0, ymm1;
    ymm0 = _mm256_loadu_pd (v1); // ymm0 = {a0, a1, a2,X}
    ymm1 = _mm256_loadu_pd (v2);	// ymm1 = {b0, b1, b2, X}
    ymm0 = _mm256_mul_pd (ymm0, ymm1); // ymm0 = {a0*b0, a1*b1, a2*b2, X}
    const __m128d valupper = _mm256_extractf128_pd(ymm0, 1); // valupper = {a2*b2, X}
    ymm1 = _mm256_castpd128_pd256 (valupper); // ymm2 = {a2*b2, X, X, X} X means undefined
    ymm0 = _mm256_hadd_pd (ymm0, ymm0); // ymm0 = {a0*b0+a1*b1, X, X, X}
    ymm0 = _mm256_add_pd (ymm0, ymm1); // ymm0 = {a0*b0+a1*b1+a2*b2, X, X, X}
    _mm256_storeu_pd (temp, ymm0);
    return *temp;
}

__attribute__((always_inline)) static inline
void scalar_triple_product(const double *u, const double *v, const double *w,double *out)
{
    cross_product(v, w, out);
    multiply_vectors(u, out, out);
}

__attribute__((always_inline)) static inline
double scalar_triple(const double *u, const double *v, const double *w)
{
    double tmp[3];
    cross_product(w, u, tmp);
    return dot_product(v, tmp);
}

#endif
