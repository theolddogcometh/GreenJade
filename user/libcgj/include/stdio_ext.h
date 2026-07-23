/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped stdio_ext.h (subset). Not GNU glibc.
 */
#pragma once

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FSETLOCKING_QUERY    0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

size_t __fbufsize(FILE *pF);
int    __flbf(FILE *pF);
size_t __fpending(FILE *pF);
void   __fpurge(FILE *pF);
void   fpurge(FILE *pF);
int    __freadable(FILE *pF);
int    __fwritable(FILE *pF);
int    __freading(FILE *pF);
int    __fwriting(FILE *pF);
int    __fsetlocking(FILE *pF, int nType);

#ifdef __cplusplus
}
#endif
