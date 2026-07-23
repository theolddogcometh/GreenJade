/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped wordexp.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WRDE_DOOFFS  (1 << 0)
#define WRDE_APPEND  (1 << 1)
#define WRDE_NOCMD   (1 << 2)
#define WRDE_REUSE   (1 << 3)
#define WRDE_SHOWERR (1 << 4)
#define WRDE_UNDEF   (1 << 5)

#define WRDE_NOSPACE 1
#define WRDE_BADCHAR 2
#define WRDE_BADVAL  3
#define WRDE_CMDSUB  4
#define WRDE_SYNTAX  5
#define WRDE_NOSYS   6

typedef struct {
    size_t  we_wordc;
    char  **we_wordv;
    size_t  we_offs;
} wordexp_t;

int  wordexp(const char *szWords, wordexp_t *pWe, int nFlags);
void wordfree(wordexp_t *pWe);

#ifdef __cplusplus
}
#endif
