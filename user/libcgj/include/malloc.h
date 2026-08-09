/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <malloc.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * GNU malloc extras: memalign, malloc_usable_size, mallinfo/malloc_stats
 * shapes as implemented. Core malloc lives in <stdlib.h>.
 *
 * Design notes
 * ------------
 * Heap is freelist bring-up; statistics may be zeroed or approximate.
 * Prefer <stdlib.h> for portable code.
 *
 * Non-goals
 * ---------
 * Full glibc ptmalloc arena tuning APIs.
 *
 * Soft residual (C2 libcgj malloc.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = memalign/malloc_usable_size/mallinfo/malloc_stats extras;
 *              freelist heap bring-up; stats may be zeroed/approximate
 *   product  = UDX/DDI + hot/cold ABI hosts; core malloc in <stdlib.h>
 *   honesty  = soft heap extras != ptmalloc product; agent PASS != Dual DoD
 *              close; stamp-free bar honesty (no version stamp); G-AC-1 no .ko AC
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: malloc soft residual Soft!=product
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
