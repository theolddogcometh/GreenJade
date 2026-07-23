/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2603: uint32_t min (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_min_u(uint32_t a, uint32_t b);
 *     - Smaller of a and b (equal → b).
 *   uint32_t __gj_u32_min_u  (alias)
 *   __libcgj_batch2603_marker = "libcgj-batch2603"
 *
 * Distinct from other min surfaces — exclusive _u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2603_marker[] = "libcgj-batch2603";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2603_min(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_min_u — return the lesser of two uint32_t values.
 *
 * a, b: operands
 * Returns the smaller value; when equal, returns b.
 */
uint32_t
gj_u32_min_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2603_min(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_min_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_min_u")));
