/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14718: u32 align-down extract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14718(void);
 *     - Returns the compile-time graph batch number for this TU (14718).
 *   uint32_t gj_u32_align_down_14718(uint32_t v, uint32_t align);
 *     - Rounds v down to the previous multiple of align. Align must be
 *       a non-zero power of two; otherwise returns v unchanged.
 *   uint32_t __gj_batch_id_14718         (alias)
 *   uint32_t __gj_u32_align_down_14718   (alias)
 *   __libcgj_batch14718_marker = "libcgj-batch14718"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior align_down helpers
 * without this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14718_marker[] = "libcgj-batch14718";

#define B14718_ID  14718u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14718_id(void)
{
	return B14718_ID;
}

static uint32_t
b14718_is_pow2(uint32_t v)
{
	return (v != 0u && (v & (v - 1u)) == 0u) ? 1u : 0u;
}

static uint32_t
b14718_align_down(uint32_t v, uint32_t align)
{
	if (b14718_is_pow2(align) == 0u)
		return v;
	return v & ~(align - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14718 - report this TU's graph batch number.
 *
 * Always returns 14718.
 */
uint32_t
gj_batch_id_14718(void)
{
	(void)NULL;
	return b14718_id();
}

/*
 * gj_u32_align_down_14718 - round v down to a multiple of align.
 *
 * Align must be a non-zero power of two; otherwise returns v.
 * Pure integer; no libc.
 */
uint32_t
gj_u32_align_down_14718(uint32_t v, uint32_t align)
{
	return b14718_align_down(v, align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14718(void)
    __attribute__((alias("gj_batch_id_14718")));

uint32_t __gj_u32_align_down_14718(uint32_t v, uint32_t align)
    __attribute__((alias("gj_u32_align_down_14718")));
