/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2601: uint64_t min (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_min_u(uint64_t a, uint64_t b);
 *     - Smaller of a and b (equal → b).
 *   uint64_t __gj_u64_min_u  (alias)
 *   __libcgj_batch2601_marker = "libcgj-batch2601"
 *
 * Distinct from gj_min_u64 (batch836) — exclusive _u name surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2601_marker[] = "libcgj-batch2601";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2601_min(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return u64A;
	}
	return u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_min_u — return the lesser of two uint64_t values.
 *
 * a, b: operands
 * Returns the smaller value; when equal, returns b.
 */
uint64_t
gj_u64_min_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2601_min(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_min_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_min_u")));
