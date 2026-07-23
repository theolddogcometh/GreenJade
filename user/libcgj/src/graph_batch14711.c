/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14711: u32 clamp extract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14711(void);
 *     - Returns the compile-time graph batch number for this TU (14711).
 *   uint32_t gj_u32_clamp_14711(uint32_t v, uint32_t lo, uint32_t hi);
 *     - Clamps v into [lo, hi]. If lo > hi, returns lo.
 *   uint32_t __gj_batch_id_14711   (alias)
 *   uint32_t __gj_u32_clamp_14711  (alias)
 *   __libcgj_batch14711_marker = "libcgj-batch14711"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior clamp helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14711_marker[] = "libcgj-batch14711";

#define B14711_ID  14711u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14711_id(void)
{
	return B14711_ID;
}

static uint32_t
b14711_clamp(uint32_t v, uint32_t lo, uint32_t hi)
{
	if (lo > hi)
		return lo;
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14711 - report this TU's graph batch number.
 *
 * Always returns 14711.
 */
uint32_t
gj_batch_id_14711(void)
{
	(void)NULL;
	return b14711_id();
}

/*
 * gj_u32_clamp_14711 - clamp v into [lo, hi].
 *
 * If lo > hi, returns lo. Pure integer; no libc.
 */
uint32_t
gj_u32_clamp_14711(uint32_t v, uint32_t lo, uint32_t hi)
{
	return b14711_clamp(v, lo, hi);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14711(void)
    __attribute__((alias("gj_batch_id_14711")));

uint32_t __gj_u32_clamp_14711(uint32_t v, uint32_t lo, uint32_t hi)
    __attribute__((alias("gj_u32_clamp_14711")));
