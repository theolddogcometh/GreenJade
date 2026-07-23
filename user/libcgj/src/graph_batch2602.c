/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2602: uint64_t max (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_max_u(uint64_t a, uint64_t b);
 *     - Greater of a and b (equal → b).
 *   uint64_t __gj_u64_max_u  (alias)
 *   __libcgj_batch2602_marker = "libcgj-batch2602"
 *
 * Distinct from gj_max_u64 (batch837) — exclusive _u name surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2602_marker[] = "libcgj-batch2602";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2602_max(uint64_t u64A, uint64_t u64B)
{
	if (u64A > u64B) {
		return u64A;
	}
	return u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_max_u — return the greater of two uint64_t values.
 *
 * a, b: operands
 * Returns the larger value; when equal, returns b.
 */
uint64_t
gj_u64_max_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2602_max(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_max_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_max_u")));
