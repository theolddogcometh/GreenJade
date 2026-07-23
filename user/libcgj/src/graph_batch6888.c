/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6888: statx mask has-all / OR / AND helpers.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_or_6888(uint32_t a, uint32_t b);
 *     - Return a | b (soft merge of two statx request masks).
 *   uint32_t gj_statx_mask_and_6888(uint32_t a, uint32_t b);
 *     - Return a & b (soft intersection / filled-bits filter).
 *   uint32_t gj_statx_mask_has_all_6888(uint32_t mask, uint32_t need);
 *     - Return 1 if (mask & need) == need (all required bits present).
 *   uint32_t __gj_statx_mask_or_6888  (alias)
 *   uint32_t __gj_statx_mask_and_6888  (alias)
 *   uint32_t __gj_statx_mask_has_all_6888  (alias)
 *   __libcgj_batch6888_marker = "libcgj-batch6888"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6888 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6888_marker[] = "libcgj-batch6888";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6888_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

static uint32_t
b6888_and(uint32_t u32A, uint32_t u32B)
{
	return u32A & u32B;
}

static uint32_t
b6888_has_all(uint32_t u32Mask, uint32_t u32Need)
{
	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_or_6888 - bitwise OR of two soft statx masks.
 *
 * a, b: soft statx request / result bitmasks (stx_mask style)
 *
 * Returns a | b. Soft pure-data merge (e.g. want = basic | btime);
 * no statx syscall. No parent wires.
 */
uint32_t
gj_statx_mask_or_6888(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6888_or(u32A, u32B);
}

/*
 * gj_statx_mask_and_6888 - bitwise AND of two soft statx masks.
 *
 * a, b: soft statx bitmasks
 *
 * Returns a & b. Soft pure-data intersect (e.g. got = stx_mask & want).
 */
uint32_t
gj_statx_mask_and_6888(uint32_t u32A, uint32_t u32B)
{
	return b6888_and(u32A, u32B);
}

/*
 * gj_statx_mask_has_all_6888 - true if mask covers all bits in need.
 *
 * mask: soft result / available bits (e.g. stx_mask after call)
 * need: required request bits
 *
 * Returns 1 when every bit of need is set in mask; else 0.
 */
uint32_t
gj_statx_mask_has_all_6888(uint32_t u32Mask, uint32_t u32Need)
{
	return b6888_has_all(u32Mask, u32Need);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_or_6888(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_statx_mask_or_6888")));

uint32_t __gj_statx_mask_and_6888(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_statx_mask_and_6888")));

uint32_t __gj_statx_mask_has_all_6888(uint32_t u32Mask, uint32_t u32Need)
    __attribute__((alias("gj_statx_mask_has_all_6888")));
