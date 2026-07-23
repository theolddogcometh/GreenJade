/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14717: u32 align-up extract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14717(void);
 *     - Returns the compile-time graph batch number for this TU (14717).
 *   uint32_t gj_u32_align_up_14717(uint32_t v, uint32_t align);
 *     - Rounds v up to the next multiple of align. Align must be a
 *       non-zero power of two; otherwise returns v unchanged. On
 *       overflow, returns 0.
 *   uint32_t __gj_batch_id_14717       (alias)
 *   uint32_t __gj_u32_align_up_14717   (alias)
 *   __libcgj_batch14717_marker = "libcgj-batch14717"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior align_up helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14717_marker[] = "libcgj-batch14717";

#define B14717_ID  14717u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14717_id(void)
{
	return B14717_ID;
}

static uint32_t
b14717_is_pow2(uint32_t v)
{
	return (v != 0u && (v & (v - 1u)) == 0u) ? 1u : 0u;
}

static uint32_t
b14717_align_up(uint32_t v, uint32_t align)
{
	uint32_t mask;
	uint32_t r;

	if (b14717_is_pow2(align) == 0u)
		return v;
	mask = align - 1u;
	r = (v + mask) & ~mask;
	/* Overflow: (v + mask) wrapped. */
	if (r < v)
		return 0u;
	return r;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14717 - report this TU's graph batch number.
 *
 * Always returns 14717.
 */
uint32_t
gj_batch_id_14717(void)
{
	(void)NULL;
	return b14717_id();
}

/*
 * gj_u32_align_up_14717 - round v up to a multiple of align.
 *
 * Align must be a non-zero power of two; otherwise returns v.
 * Overflow returns 0. Pure integer; no libc.
 */
uint32_t
gj_u32_align_up_14717(uint32_t v, uint32_t align)
{
	return b14717_align_up(v, align);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14717(void)
    __attribute__((alias("gj_batch_id_14717")));

uint32_t __gj_u32_align_up_14717(uint32_t v, uint32_t align)
    __attribute__((alias("gj_u32_align_up_14717")));
