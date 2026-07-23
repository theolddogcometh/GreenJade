/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4275: checked uint64_t sub with overflow flag (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_sub_overflow_u(uint64_t a, uint64_t b, uint64_t *out);
 *     - Compute a - b. Return 1 if a < b (unsigned underflow / overflow
 *       of the difference), else 0. When out is non-NULL, write the
 *       wrapping difference. out may be NULL (status-only; still return
 *       the overflow flag).
 *   int __gj_u64_sub_overflow_u  (alias)
 *   __libcgj_batch4275_marker = "libcgj-batch4275"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_u32_sub_overflow_u (batch4272) / gj_u64_sub_sat
 * (batch2032) — unique gj_u64_sub_overflow_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4275_marker[] = "libcgj-batch4275";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Underflow / overflow iff a < b. Always compute wrapping difference;
 * optional store.
 */
static int
b4275_sub_ov(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	uint64_t u64Diff;
	int nOv;

	nOv = (u64A < u64B) ? 1 : 0;
	u64Diff = u64A - u64B;
	if (pOut != NULL) {
		*pOut = u64Diff;
	}
	return nOv;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_overflow_u - subtract a - b with overflow detection.
 *
 * a, b: minuend / subtrahend
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a < b, else 0. Writes wrapping difference to *out when
 * out is non-NULL. No parent wires.
 */
int
gj_u64_sub_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
{
	(void)NULL;
	return b4275_sub_ov(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_sub_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_sub_overflow_u")));
