/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3262: map uint32_t across two ranges (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_map_range_u(uint32_t x, uint32_t in_lo, uint32_t in_hi,
 *       uint32_t out_lo, uint32_t out_hi);
 *     - Linearly map x from the closed interval between in_lo and in_hi
 *       onto the closed interval from out_lo toward out_hi. x is clamped
 *       into the ordered source span first. Degenerate source span
 *       (in_lo == in_hi) yields out_lo. Destination direction is
 *       preserved when out_lo > out_hi.
 *   uint32_t __gj_u32_map_range_u  (alias)
 *   __libcgj_batch3262_marker = "libcgj-batch3262"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u32_map_range_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3262_marker[] = "libcgj-batch3262";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Linear map of x from [in_lo, in_hi] (direction-aware) onto
 * [out_lo → out_hi]. Source endpoints may be inverted; x is clamped to
 * the ordered source span. Product uses uint64_t intermediate.
 */
static uint32_t
b3262_map_range(uint32_t u32X, uint32_t u32InLo, uint32_t u32InHi,
    uint32_t u32OutLo, uint32_t u32OutHi)
{
	uint32_t u32SrcLo;
	uint32_t u32SrcHi;
	uint32_t u32InSpan;
	uint32_t u32OutSpan;
	uint32_t u32Off;
	uint32_t u32Mix;
	uint64_t u64Prod;

	if (u32InLo == u32InHi) {
		return u32OutLo;
	}

	if (u32InLo < u32InHi) {
		u32SrcLo = u32InLo;
		u32SrcHi = u32InHi;
	} else {
		u32SrcLo = u32InHi;
		u32SrcHi = u32InLo;
	}

	if (u32X < u32SrcLo) {
		u32X = u32SrcLo;
	} else if (u32X > u32SrcHi) {
		u32X = u32SrcHi;
	}

	/* Distance along source from in_lo toward in_hi. */
	if (u32InHi > u32InLo) {
		u32InSpan = u32InHi - u32InLo;
		u32Off = u32X - u32InLo;
	} else {
		u32InSpan = u32InLo - u32InHi;
		u32Off = u32InLo - u32X;
	}

	if (u32OutHi >= u32OutLo) {
		u32OutSpan = u32OutHi - u32OutLo;
		u64Prod = (uint64_t)u32Off * (uint64_t)u32OutSpan;
		u32Mix = (uint32_t)(u64Prod / (uint64_t)u32InSpan);
		return u32OutLo + u32Mix;
	}
	u32OutSpan = u32OutLo - u32OutHi;
	u64Prod = (uint64_t)u32Off * (uint64_t)u32OutSpan;
	u32Mix = (uint32_t)(u64Prod / (uint64_t)u32InSpan);
	return u32OutLo - u32Mix;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_map_range_u - map x from one uint32 range onto another.
 *
 * x:               value to map (clamped into ordered source span)
 * in_lo, in_hi:    source endpoints (may be inverted)
 * out_lo, out_hi:  destination endpoints (direction preserved)
 *
 * Degenerate source (in_lo == in_hi) returns out_lo. No parent wires.
 */
uint32_t
gj_u32_map_range_u(uint32_t u32X, uint32_t u32InLo, uint32_t u32InHi,
    uint32_t u32OutLo, uint32_t u32OutHi)
{
	(void)NULL;
	return b3262_map_range(u32X, u32InLo, u32InHi, u32OutLo, u32OutHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_map_range_u(uint32_t u32X, uint32_t u32InLo,
    uint32_t u32InHi, uint32_t u32OutLo, uint32_t u32OutHi)
    __attribute__((alias("gj_u32_map_range_u")));
