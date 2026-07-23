/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped malloc.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mallinfo {
    int arena;
    int ordblks;
    int smblks;
    int hblks;
    int hblkhd;
    int usmblks;
    int fsmblks;
    int uordblks;
    int fordblks;
    int keepcost;
};

struct mallinfo2 {
    size_t arena;
    size_t ordblks;
    size_t smblks;
    size_t hblks;
    size_t hblkhd;
    size_t usmblks;
    size_t fsmblks;
    size_t uordblks;
    size_t fordblks;
    size_t keepcost;
};

struct mallinfo  mallinfo(void);
struct mallinfo2 mallinfo2(void);
int              mallopt(int nParam, int nVal);
void             malloc_stats(void);
int              malloc_info(int nOptions, FILE *pF);
int              malloc_trim(size_t cbPad);
size_t           malloc_usable_size(void *p);

#define M_MXFAST  1
#define M_TRIM_THRESHOLD -1
#define M_TOP_PAD -2
#define M_MMAP_THRESHOLD -3
#define M_MMAP_MAX -4
#define M_CHECK_ACTION -5
#define M_PERTURB -6
#define M_ARENA_TEST -7
#define M_ARENA_MAX -8

#ifdef __cplusplus
}
#endif
