/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5083: debug "likely" nonzero predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_likely_u(uint32_t x);
 *     - Return 1 if x != 0, else 0. Documents that the nonzero (true)
 *       path is the expected / hot path for branch-hint consumers.
 *   uint32_t __gj_dbg_likely_u  (alias)
 *   __libcgj_batch5083_marker = "libcgj-batch5083"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_likely_u surface only; no multi-def. Distinct from
 * gj_likely (batch983, int identity). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5083_marker[] = "libcgj-batch5083";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5083_likely(uint32_t u32X)
{
	if (u32X != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_likely_u - 1 if x is nonzero (hot/true path), else 0.
 *
 * x: value under test
 *
 * Booleanizes x as (x != 0). Pair of gj_dbg_unlikely_u (batch5084).
 * No parent wires.
 */
uint32_t
gj_dbg_likely_u(uint32_t u32X)
{
	(void)NULL;
	return b5083_likely(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_likely_u(uint32_t u32X)
    __attribute__((alias("gj_dbg_likely_u")));
