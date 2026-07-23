/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3908: nonzero-divisor u64 quotient (quot_nz_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_quot_nz_u(uint64_t a, uint64_t b);
 *     - a / b as unsigned floor quotient (intended nonzero divisor).
 *       b == 0 -> 0 (guard; avoids UB). a == 0 -> 0 for any b.
 *   uint64_t __gj_u64_quot_nz_u  (alias)
 *   __libcgj_batch3908_marker = "libcgj-batch3908"
 *
 * Exclusive continuum CREATE-ONLY (3901-3910). Distinct from
 * gj_u64_fdiv_u (batch3904) / gj_u64_div_floor_u (batch3004) — unique
 * gj_u64_quot_nz_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3908_marker[] = "libcgj-batch3908";

/* ---- freestanding helpers ---------------------------------------------- */

/* a / b; b == 0 yields 0. */
static uint64_t
b3908_quot(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return u64A / u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_quot_nz_u - unsigned quotient a / b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 -> 0; nonzero path is the normal case)
 * No parent wires.
 */
uint64_t
gj_u64_quot_nz_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3908_quot(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_quot_nz_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_quot_nz_u")));
