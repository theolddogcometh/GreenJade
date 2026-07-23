/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2356: uint64_t three-way minimum
 * (post-2350 exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_min3(uint64_t a, uint64_t b, uint64_t c);
 *     - Least of a, b, and c (unsigned compare). Pairwise fold:
 *       min(a, b) then min(that, c).
 *   uint64_t __gj_u64_min3  (alias)
 *   __libcgj_batch2356_marker = "libcgj-batch2356"
 *
 * Distinct from gj_min3_u64 (batch588) - unique gj_u64_min3
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2356_marker[] = "libcgj-batch2356";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least of three uint64 values. */
static uint64_t
b2356_min3(uint64_t uA, uint64_t uB, uint64_t uC)
{
	uint64_t uM;

	if (uA < uB) {
		uM = uA;
	} else {
		uM = uB;
	}
	if (uC < uM) {
		return uC;
	}
	return uM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_min3 - return the least of three uint64_t values.
 *
 * a, b, c: candidates
 *
 * Pairwise reduction under unsigned compare. Does not call libc.
 */
uint64_t
gj_u64_min3(uint64_t uA, uint64_t uB, uint64_t uC)
{
	(void)NULL;
	return b2356_min3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_min3(uint64_t uA, uint64_t uB, uint64_t uC)
    __attribute__((alias("gj_u64_min3")));
