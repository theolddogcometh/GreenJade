/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3266: wrap uint32_t into a closed range (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_wrap_range_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Fold x into the closed interval [lo, hi] by modular wrap.
 *       If lo > hi, bounds are swapped. If lo == hi, returns lo.
 *       Full-domain [0, UINT32_MAX] returns x unchanged. Otherwise
 *       returns lo + ((x - lo) mod (hi - lo + 1)).
 *   uint32_t __gj_u32_wrap_range_u  (alias)
 *   __libcgj_batch3266_marker = "libcgj-batch3266"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u32_wrap_range_u surface only; no multi-def. Distinct from
 * gj_u32_clamp_u (batch2703) / wrap-add helpers. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3266_marker[] = "libcgj-batch3266";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Modular wrap of x into the closed interval [lo, hi].
 * (x - lo) is computed in unsigned modular arithmetic so values below
 * lo wrap correctly without signed types.
 */
static uint32_t
b3266_wrap_range(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	uint32_t u32Tmp;
	uint32_t u32Span;
	uint32_t u32Off;

	if (u32Lo > u32Hi) {
		u32Tmp = u32Lo;
		u32Lo = u32Hi;
		u32Hi = u32Tmp;
	}

	if (u32Lo == u32Hi) {
		return u32Lo;
	}

	/* Full domain: span would overflow to 0. */
	if (u32Lo == 0u && u32Hi == UINT32_MAX) {
		return u32X;
	}

	u32Span = (u32Hi - u32Lo) + 1u;
	u32Off = u32X - u32Lo;
	return u32Lo + (u32Off % u32Span);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_wrap_range_u - fold x into the closed interval [lo, hi].
 *
 * x:  value to wrap
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns a value in [min(lo,hi), max(lo,hi)] by modular wrap of the
 * closed span. No parent wires.
 */
uint32_t
gj_u32_wrap_range_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b3266_wrap_range(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_wrap_range_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u32_wrap_range_u")));
