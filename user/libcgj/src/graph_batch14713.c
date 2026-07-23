/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14713: u32 saturating add (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14713(void);
 *     - Returns the compile-time graph batch number for this TU (14713).
 *   uint32_t gj_u32_sat_add_14713(uint32_t a, uint32_t b);
 *     - Returns a + b, saturating at UINT32_MAX on overflow.
 *   uint32_t __gj_batch_id_14713     (alias)
 *   uint32_t __gj_u32_sat_add_14713  (alias)
 *   __libcgj_batch14713_marker = "libcgj-batch14713"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior sat_add helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14713_marker[] = "libcgj-batch14713";

#define B14713_ID  14713u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14713_id(void)
{
	return B14713_ID;
}

static uint32_t
b14713_sat_add(uint32_t a, uint32_t b)
{
	uint32_t s;

	s = a + b;
	if (s < a)
		return UINT32_MAX;
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14713 - report this TU's graph batch number.
 *
 * Always returns 14713.
 */
uint32_t
gj_batch_id_14713(void)
{
	(void)NULL;
	return b14713_id();
}

/*
 * gj_u32_sat_add_14713 - saturating unsigned 32-bit addition.
 *
 * Overflow clamps to UINT32_MAX. Pure integer; no libc.
 */
uint32_t
gj_u32_sat_add_14713(uint32_t a, uint32_t b)
{
	return b14713_sat_add(a, b);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14713(void)
    __attribute__((alias("gj_batch_id_14713")));

uint32_t __gj_u32_sat_add_14713(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_sat_add_14713")));
