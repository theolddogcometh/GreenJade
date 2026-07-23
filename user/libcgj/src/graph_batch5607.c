/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5607: GPU clock min/max (clamp / in-range).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_clock_clamp_mhz_5607(uint32_t mhz, uint32_t lo, uint32_t hi);
 *     - Clamp mhz into [lo, hi]; if lo > hi, returns lo.
 *   uint32_t gj_gpu_clock_in_range_5607(uint32_t mhz, uint32_t lo, uint32_t hi);
 *     - Return 1 when lo <= mhz <= hi, else 0.
 *   uint32_t gj_gpu_clock_span_mhz_5607(uint32_t lo, uint32_t hi);
 *     - Return hi - lo when hi >= lo, else 0.
 *   uint32_t gj_gpu_clock_ready_5607(void);
 *     - Returns 1 (soft GPU clock path ready lamp).
 *   uint32_t __gj_gpu_clock_clamp_mhz_5607  (alias)
 *   uint32_t __gj_gpu_clock_in_range_5607  (alias)
 *   uint32_t __gj_gpu_clock_span_mhz_5607  (alias)
 *   uint32_t __gj_gpu_clock_ready_5607  (alias)
 *   __libcgj_batch5607_marker = "libcgj-batch5607"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). GPU clock clamp helpers;
 * distinct from gj_gpu_clock_*_5606. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5607_marker[] = "libcgj-batch5607";

#define B5607_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5607_clamp(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
{
	if (u32Lo > u32Hi) {
		return u32Lo;
	}
	if (u32Mhz < u32Lo) {
		return u32Lo;
	}
	if (u32Mhz > u32Hi) {
		return u32Hi;
	}
	return u32Mhz;
}

static uint32_t
b5607_in_range(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
{
	if (u32Mhz < u32Lo) {
		return 0u;
	}
	if (u32Mhz > u32Hi) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b5607_span(uint32_t u32Lo, uint32_t u32Hi)
{
	if (u32Hi < u32Lo) {
		return 0u;
	}
	return u32Hi - u32Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_clock_clamp_mhz_5607 - soft clamp GPU clock into [lo, hi].
 *
 * mhz: candidate clock MHz
 * lo: floor MHz
 * hi: ceiling MHz
 *
 * Returns clamped value; if lo > hi, returns lo.
 */
uint32_t
gj_gpu_clock_clamp_mhz_5607(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b5607_clamp(u32Mhz, u32Lo, u32Hi);
}

/*
 * gj_gpu_clock_in_range_5607 - soft test mhz in [lo, hi].
 *
 * Returns 1 when lo <= mhz <= hi, else 0.
 */
uint32_t
gj_gpu_clock_in_range_5607(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
{
	return b5607_in_range(u32Mhz, u32Lo, u32Hi);
}

/*
 * gj_gpu_clock_span_mhz_5607 - soft span of allowed clock band.
 *
 * Returns hi - lo when hi >= lo, else 0.
 */
uint32_t
gj_gpu_clock_span_mhz_5607(uint32_t u32Lo, uint32_t u32Hi)
{
	return b5607_span(u32Lo, u32Hi);
}

/*
 * gj_gpu_clock_ready_5607 - soft GPU clock path ready lamp.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_clock_ready_5607(void)
{
	return B5607_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_clock_clamp_mhz_5607(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_gpu_clock_clamp_mhz_5607")));

uint32_t __gj_gpu_clock_in_range_5607(uint32_t u32Mhz, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_gpu_clock_in_range_5607")));

uint32_t __gj_gpu_clock_span_mhz_5607(uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_gpu_clock_span_mhz_5607")));

uint32_t __gj_gpu_clock_ready_5607(void)
    __attribute__((alias("gj_gpu_clock_ready_5607")));
