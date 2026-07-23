/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4274: checked uint64_t add with overflow flag (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_add_overflow_u(uint64_t a, uint64_t b, uint64_t *out);
 *     - Compute a + b. Always return 1 if the mathematical sum overflows
 *       uint64_t, else 0. When out is non-NULL, write the wrapping sum.
 *       out may be NULL (status-only; still return the overflow flag).
 *   int __gj_u64_add_overflow_u  (alias)
 *   __libcgj_batch4274_marker = "libcgj-batch4274"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_add_overflow_u64 (batch427) / gj_u64_add_check
 * (batch647) / gj_u32_add_overflow_u (batch4271) — unique surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4274_marker[] = "libcgj-batch4274";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Overflow iff sum wraps below a (equivalently b > UINT64_MAX - a).
 * Always compute wrapping sum; optional store.
 */
static int
b4274_add_ov(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	uint64_t u64Sum;
	int nOv;

	u64Sum = u64A + u64B;
	nOv = (u64Sum < u64A) ? 1 : 0;
	if (pOut != NULL) {
		*pOut = u64Sum;
	}
	return nOv;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_add_overflow_u - add a + b with overflow detection.
 *
 * a, b: addends
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a + b overflows uint64_t, else 0. Writes wrapping sum
 * to *out when out is non-NULL. No parent wires.
 */
int
gj_u64_add_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
{
	(void)NULL;
	return b4274_add_ov(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_add_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_add_overflow_u")));
