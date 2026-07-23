/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3267: wrap uint64_t into a closed range (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_wrap_range_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Fold x into the closed interval [lo, hi] by modular wrap.
 *       If lo > hi, bounds are swapped. If lo == hi, returns lo.
 *       Full-domain [0, UINT64_MAX] returns x unchanged. Otherwise
 *       returns lo + ((x - lo) mod (hi - lo + 1)).
 *   uint64_t __gj_u64_wrap_range_u  (alias)
 *   __libcgj_batch3267_marker = "libcgj-batch3267"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u64_wrap_range_u surface only; no multi-def. Distinct from
 * gj_u32_wrap_range_u (batch3266) / gj_u64_wrap_add (batch738). No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3267_marker[] = "libcgj-batch3267";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Modular wrap of x into the closed interval [lo, hi].
 * (x - lo) is unsigned modular so values below lo wrap correctly.
 */
static uint64_t
b3267_wrap_range(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Tmp;
	uint64_t u64Span;
	uint64_t u64Off;

	if (u64Lo > u64Hi) {
		u64Tmp = u64Lo;
		u64Lo = u64Hi;
		u64Hi = u64Tmp;
	}

	if (u64Lo == u64Hi) {
		return u64Lo;
	}

	/* Full domain: span would overflow to 0. */
	if (u64Lo == 0ull && u64Hi == UINT64_MAX) {
		return u64X;
	}

	u64Span = (u64Hi - u64Lo) + 1ull;
	u64Off = u64X - u64Lo;
	return u64Lo + (u64Off % u64Span);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_wrap_range_u - fold x into the closed interval [lo, hi].
 *
 * x:  value to wrap
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns a value in [min(lo,hi), max(lo,hi)] by modular wrap of the
 * closed span. No parent wires.
 */
uint64_t
gj_u64_wrap_range_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b3267_wrap_range(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_wrap_range_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_wrap_range_u")));
