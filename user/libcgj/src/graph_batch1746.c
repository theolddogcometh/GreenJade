/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1746: percent-of-u64 resource share.
 *
 * Surface (unique symbols):
 *   uint64_t gj_percent_of_u64(uint64_t x, uint64_t pct);
 *     — Return (x * pct) / 100 for resource share math. Overflow-safe
 *       path when x * pct would exceed UINT64_MAX (divide-first).
 *   uint64_t __gj_percent_of_u64  (alias)
 *   __libcgj_batch1746_marker = "libcgj-batch1746"
 *
 * Distinct from gj_pct_of_u64 (batch996) — separate name with a safe
 * overflow path. Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1746_marker[] = "libcgj-batch1746";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1746_pct(uint64_t u64X, uint64_t u64Pct)
{
	if (u64Pct == 0ull || u64X == 0ull) {
		return 0ull;
	}
	if (u64X > UINT64_MAX / u64Pct) {
		return (u64X / 100ull) * u64Pct +
		    ((u64X % 100ull) * u64Pct) / 100ull;
	}
	return (u64X * u64Pct) / 100ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_percent_of_u64 — compute (x * pct) / 100 for resource budgets.
 *
 * u64X:   base resource quantity (bytes, shares, ticks, ...)
 * u64Pct: percentage (100 = 100%)
 *
 * Overflow-safe integer path; pure arithmetic, no syscalls.
 */
uint64_t
gj_percent_of_u64(uint64_t u64X, uint64_t u64Pct)
{
	return b1746_pct(u64X, u64Pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_percent_of_u64(uint64_t u64X, uint64_t u64Pct)
    __attribute__((alias("gj_percent_of_u64")));
