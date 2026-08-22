/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <math.h> for libcgj. OpenSSH HAVE_DECL_INFINITY
 * plus log/exp/pow bring-up (no libm.so). Dual DoD B OPEN.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HUGE_VAL
#define HUGE_VAL __builtin_huge_val()
#endif
#ifndef HUGE_VALF
#define HUGE_VALF __builtin_huge_valf()
#endif
#ifndef HUGE_VALL
#define HUGE_VALL __builtin_huge_vall()
#endif
#ifndef INFINITY
#define INFINITY __builtin_inff()
#endif
#ifndef NAN
#define NAN __builtin_nanf("")
#endif
#ifndef FP_NAN
#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4
#endif

#ifndef fpclassify
#define fpclassify(x) \
    __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, \
                         FP_ZERO, (x))
#endif
#ifndef isnan
#define isnan(x) __builtin_isnan(x)
#endif
#ifndef isinf
#define isinf(x) __builtin_isinf(x)
#endif
#ifndef isfinite
#define isfinite(x) __builtin_isfinite(x)
#endif
#ifndef isnormal
#define isnormal(x) __builtin_isnormal(x)
#endif
#ifndef signbit
#define signbit(x) __builtin_signbit(x)
#endif
#ifndef isgreater
#define isgreater(x, y)       __builtin_isgreater((x), (y))
#define isgreaterequal(x, y)  __builtin_isgreaterequal((x), (y))
#define isless(x, y)          __builtin_isless((x), (y))
#define islessequal(x, y)     __builtin_islessequal((x), (y))
#define islessgreater(x, y)   __builtin_islessgreater((x), (y))
#define isunordered(x, y)     __builtin_isunordered((x), (y))
#endif
#ifndef MATH_ERRNO
#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2
#endif
#ifndef math_errhandling
#define math_errhandling MATH_ERRNO
#endif

#ifndef FP_ILOGB0
#define FP_ILOGB0   (-2147483647 - 1)
#endif
#ifndef FP_ILOGBNAN
#define FP_ILOGBNAN 2147483647
#endif

#ifndef M_E
#define M_E         2.7182818284590452354
#endif
#ifndef M_LOG2E
#define M_LOG2E     1.4426950408889634074
#endif
#ifndef M_LOG10E
#define M_LOG10E    0.43429448190325182765
#endif
#ifndef M_LN2
#define M_LN2       0.69314718055994530942
#endif
#ifndef M_LN10
#define M_LN10      2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif
#ifndef M_SQRT2
#define M_SQRT2     1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2   0.70710678118654752440
#endif

double sqrt(double dX);
double fabs(double dX);
double floor(double dX);
double ceil(double dX);
double trunc(double dX);
double ldexp(double dX, int nExp);
double frexp(double dX, int *pExp);
double modf(double dX, double *pInt);
double fmod(double dX, double dY);
double copysign(double dX, double dY);
double scalbn(double dX, int nExp);
int    ilogb(double dX);
double round(double dX);
double log(double dX);
double log2(double dX);
double log10(double dX);
double logb(double dX);
double exp(double dX);
double exp2(double dX);
double pow(double dX, double dY);
double hypot(double dX, double dY);
double cbrt(double dX);
double rint(double dX);
double nearbyint(double dX);
double nextafter(double dX, double dY);
double fmin(double dX, double dY);
double fmax(double dX, double dY);
double fdim(double dX, double dY);
double remainder(double dX, double dY);
double log1p(double dX);
double expm1(double dX);
int    __isnan(double dX);
int    __isinf(double dX);
int    __finite(double dX);
int    __isnanf(float fX);
int    __isinff(float fX);
int    __finitef(float fX);
float  sqrtf(float fX);
float  fabsf(float fX);
float  floorf(float fX);
float  ceilf(float fX);
float  truncf(float fX);
float  ldexpf(float fX, int nExp);
float  frexpf(float fX, int *pExp);
float  modff(float fX, float *pInt);
float  fmodf(float fX, float fY);
float  copysignf(float fX, float fY);
float  scalbnf(float fX, int nExp);
float  roundf(float fX);
float  logf(float fX);
float  log2f(float fX);
float  log10f(float fX);
float  logbf(float fX);
float  expf(float fX);
float  exp2f(float fX);
float  powf(float fX, float fY);
float  hypotf(float fX, float fY);
float  cbrtf(float fX);
float  rintf(float fX);
float  nearbyintf(float fX);
float  nextafterf(float fX, float fY);
float  fminf(float fX, float fY);
float  fmaxf(float fX, float fY);
float  fdimf(float fX, float fY);
float  remainderf(float fX, float fY);
float  log1pf(float fX);
float  expm1f(float fX);
double sin(double dX);
double cos(double dX);
double tan(double dX);
double atan(double dX);
double atan2(double dY, double dX);
double asin(double dX);
double acos(double dX);
float  sinf(float fX);
float  cosf(float fX);
float  tanf(float fX);
float  atanf(float fX);
float  atan2f(float fY, float fX);
float  asinf(float fX);
float  acosf(float fX);
long   lround(double dX);
long   lrint(double dX);
long long llround(double dX);
long long llrint(double dX);
long   lroundf(float fX);
long   lrintf(float fX);
long long llroundf(float fX);
long long llrintf(float fX);
double fma(double dX, double dY, double dZ);
float  fmaf(float fX, float fY, float fZ);
double sinh(double dX);
double cosh(double dX);
double tanh(double dX);
double asinh(double dX);
double acosh(double dX);
double atanh(double dX);
float  sinhf(float fX);
float  coshf(float fX);
float  tanhf(float fX);
float  asinhf(float fX);
float  acoshf(float fX);
float  atanhf(float fX);
double erf(double dX);
double erfc(double dX);
double tgamma(double dX);
double lgamma(double dX);
extern int signgam;
double nan(const char *sz);
float  nanf(const char *sz);
double scalbln(double dX, long nExp);
double nexttoward(double dX, long double dY);
double remquo(double dX, double dY, int *pQuo);
int    finite(double dX);
int    finitef(float fX);
void   sincos(double dX, double *pSin, double *pCos);
void   sincosf(float fX, float *pSin, float *pCos);
double exp10(double dX);
float  exp10f(float fX);
double drem(double dX, double dY);
double gamma(double dX); /* SVID: same as lgamma */
typedef float  float_t;
typedef double double_t;

#ifdef __cplusplus
}
#endif
