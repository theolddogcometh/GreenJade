/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch572 (renamed surface to avoid multi-def with earlier batch): n! for n fitting in uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_factorial_u64(unsigned n);
 *     — Product 1*2*...*n. 0! and 1! are 1. Returns 0 when n > 20
 *       (21! overflows uint64_t; 20! == 2432902008176640000).
 *   uint64_t __gj_factorial_u64  (alias)
 *   __libcgj_batch572_marker = "libcgj-batch572"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * floating point, no libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch572_marker[] = "libcgj-batch572";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_factorial_u64 — n! for n in [0, 20]; 0 on overflow (n > 20).
 *
 * 20! is the largest factorial that fits in uint64_t. Progressive
 * multiply from 2..n is exact in that range with no intermediate wrap.
 */
uint64_t
gj_factorial_u64(unsigned uN)
{
	uint64_t u64Prod;
	unsigned uI;

	if (uN > 20u) {
		return 0u;
	}

	u64Prod = 1u;
	for (uI = 2u; uI <= uN; uI++) {
		u64Prod *= (uint64_t)uI;
	}
	return u64Prod;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_factorial_u64(unsigned uN)
    __attribute__((alias("gj_factorial_u64")));
