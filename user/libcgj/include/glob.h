/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped glob.h (basic * and ?). Not GNU glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GLOB_ERR      (1 << 0)
#define GLOB_MARK     (1 << 1)
#define GLOB_NOSORT   (1 << 2)
#define GLOB_DOOFFS   (1 << 3)
#define GLOB_NOCHECK  (1 << 4)
#define GLOB_APPEND   (1 << 5)
#define GLOB_NOESCAPE (1 << 6)
#define GLOB_PERIOD   (1 << 7)
#define GLOB_TILDE    (1 << 12)
#define GLOB_ONLYDIR  (1 << 13)

#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
#define GLOB_NOSYS   4

typedef struct {
    size_t   gl_pathc;
    char   **gl_pathv;
    size_t   gl_offs;
    int      gl_flags;
} glob_t;

int  glob(const char *szPattern, int nFlags,
          int (*pfnErr)(const char *, int), glob_t *pGlob);
void globfree(glob_t *pGlob);

#ifdef __cplusplus
}
#endif
