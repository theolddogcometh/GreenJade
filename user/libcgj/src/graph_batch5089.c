/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5089: debug nonzero value predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_not_zero_u(uint32_t x);
 *     - Return 1 if x != 0, else 0. Explicit nonzero assert probe for
 *       freestanding debug paths.
 *   uint32_t __gj_dbg_not_zero_u  (alias)
 *   __libcgj_batch5089_marker = "libcgj-batch5089"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_not_zero_u surface only; no multi-def. Distinct from
 * gj_dbg_likely_u / gj_dbg_assert_ok_u (same numeric form, separate
 * call sites / symbol surface). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5089_marker[] = "libcgj-batch5089";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5089_not_zero(uint32_t u32X)
{
	if (u32X != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_not_zero_u - 1 if x is nonzero, else 0.
 *
 * x: value under test
 *
 * Explicit nonzero probe. No parent wires.
 */
uint32_t
gj_dbg_not_zero_u(uint32_t u32X)
{
	(void)NULL;
	return b5089_not_zero(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_not_zero_u(uint32_t u32X)
    __attribute__((alias("gj_dbg_not_zero_u")));
