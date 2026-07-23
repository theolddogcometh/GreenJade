/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1512: constant-time select between two uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ct_cmov_u32(int cond, uint32_t a, uint32_t b);
 *     — Branch-free: cond != 0 → a, else b.
 *   uint32_t __gj_ct_cmov_u32  (alias)
 *   __libcgj_batch1512_marker = "libcgj-batch1512"
 *
 * Avoid multi-def: gj_ct_select_u32 / __gj_ct_select_u32 live in
 * graph_batch1252.c; gj_select_u32 lives in graph_batch120.c. This TU
 * exports only the cmov-named surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No real crypto copied.
 */

#include <stdint.h>

const char __libcgj_batch1512_marker[] = "libcgj-batch1512";

/*
 * gj_ct_cmov_u32 — branch-free select between two uint32_t values.
 *
 * cond != 0 → a; cond == 0 → b.
 */
uint32_t
gj_ct_cmov_u32(int nCond, uint32_t u32A, uint32_t u32B)
{
	uint32_t u32C;
	uint32_t u32Mask;

	u32C = (uint32_t)(nCond != 0);
	u32Mask = 0u - u32C;

	return (u32A & u32Mask) | (u32B & ~u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ct_cmov_u32(int nCond, uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_ct_cmov_u32")));
