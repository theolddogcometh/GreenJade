/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1252: constant-time select between two uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ct_select_u32(int cond, uint32_t a, uint32_t b);
 *     — Branch-free: cond != 0 → a, else b.
 *   uint32_t __gj_ct_select_u32  (alias)
 *   __libcgj_batch1252_marker = "libcgj-batch1252"
 *
 * Distinct from gj_select_u32 (batch120) — unique ct_ name surface.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1252_marker[] = "libcgj-batch1252";

/*
 * gj_ct_select_u32 — branch-free select between two uint32_t values.
 *
 * cond != 0 → a; cond == 0 → b.
 */
uint32_t
gj_ct_select_u32(int nCond, uint32_t u32A, uint32_t u32B)
{
	uint32_t u32C;
	uint32_t u32Mask;

	u32C = (uint32_t)(nCond != 0);
	u32Mask = 0u - u32C;

	return (u32A & u32Mask) | (u32B & ~u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ct_select_u32(int nCond, uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_ct_select_u32")));
