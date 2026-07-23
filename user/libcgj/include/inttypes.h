/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped inttypes.h (subset). Not GNU glibc.
 */
#pragma once

#include <stdint.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;

intmax_t  imaxabs(intmax_t n);
imaxdiv_t imaxdiv(intmax_t nNumer, intmax_t nDenom);
intmax_t  strtoimax(const char *sz, char **ppEnd, int nBase);
uintmax_t strtoumax(const char *sz, char **ppEnd, int nBase);
intmax_t  wcstoimax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase);
uintmax_t wcstoumax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase);

/* printf/scanf macros (common subset) */
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#define SCNd64 "ld"
#define SCNu64 "lu"

#ifdef __cplusplus
}
#endif
