#ifndef __RAY_MATH_TOOLKIT_H
#define __RAY_MATH_TOOLKIT_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <emmintrin.h>
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

__attribute__((always_inline)) static inline
double dot_product(const double *v1, const double *v2)
{
    __m128d I0 = _mm_loadu_pd(v1);
    __m128d I1 = _mm_loadu_pd(v1+1);
    __m128d I2 = _mm_loadu_pd(v2);
    __m128d I3 = _mm_loadu_pd(v2+1);

    __m128d T1 = _mm_mul_pd(I0,I2);
    __m128d T2 = _mm_mul_pd(I1,I3);
    __m128d T3 = _mm_unpackhi_pd(T2,T2);

    __m128d T4 = _mm_add_pd(T1,T2);
    __m128d T5 = _mm_add_pd(T3,T4);
    return _mm_cvtsd_f64(T5);
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
