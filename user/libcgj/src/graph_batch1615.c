/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1615: timeslice from nice value.
 *
 * Surface (unique symbols):
 *   uint32_t gj_timeslice_from_nice(int32_t nice, uint32_t base);
 *     — Scale base timeslice by nice. nice is clamped to [-20, 19].
 *       At nice 0 returns base (or 1 if base == 0). Lower nice (more
 *       favored) yields a larger slice: base * (40 - nice_u) / 20 with
 *       nice_u = nice + 20. Result is at least 1 when base > 0.
 *   uint32_t __gj_timeslice_from_nice  (alias)
 *   __libcgj_batch1615_marker = "libcgj-batch1615"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1615_marker[] = "libcgj-batch1615";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1615_clamp_nice(int32_t iNice)
{
	if (iNice < -20) {
		return -20;
	}
	if (iNice > 19) {
		return 19;
	}
	return iNice;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeslice_from_nice — pure timeslice hint from a nice value.
 *
 * Classic O(1)-scheduler style scaling around a caller-supplied base
 * quantum. Pure data — no global HZ, no policy objects.
 */
uint32_t
gj_timeslice_from_nice(int32_t iNice, uint32_t u32Base)
{
	uint32_t u32NiceU;
	uint32_t u32Num;
	uint32_t u32Out;

	if (u32Base == 0u) {
		return 0u;
	}

	iNice = b1615_clamp_nice(iNice);
	u32NiceU = (uint32_t)(iNice + 20); /* 0..39 */

	/*
	 * slice = base * (40 - nice_u) / 20
	 * nice -20 → 2*base; nice 0 → base; nice 19 → base*21/20
	 */
	u32Num = 40u - u32NiceU;
	/* base * u32Num fits in 64-bit for any uint32 base. */
	u32Out = (uint32_t)(((uint64_t)u32Base * (uint64_t)u32Num) / 20ull);
	if (u32Out == 0u) {
		return 1u;
	}
	return u32Out;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timeslice_from_nice(int32_t iNice, uint32_t u32Base)
    __attribute__((alias("gj_timeslice_from_nice")));
