/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4272: checked uint32_t sub with overflow flag (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_sub_overflow_u(uint32_t a, uint32_t b, uint32_t *out);
 *     - Compute a - b. Return 1 if a < b (unsigned underflow / overflow
 *       of the difference), else 0. When out is non-NULL, write the
 *       wrapping difference. out may be NULL (status-only; still return
 *       the overflow flag).
 *   int __gj_u32_sub_overflow_u  (alias)
 *   __libcgj_batch4272_marker = "libcgj-batch4272"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_u32_add_overflow_u (batch4271) / gj_u64_sub_sat
 * (batch2032) — unique gj_u32_sub_overflow_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4272_marker[] = "libcgj-batch4272";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Underflow / overflow iff a < b. Always compute wrapping difference;
 * optional store.
 */
static int
b4272_sub_ov(uint32_t u32A, uint32_t u32B, uint32_t *pOut)
{
	uint32_t u32Diff;
	int nOv;

	nOv = (u32A < u32B) ? 1 : 0;
	u32Diff = u32A - u32B;
	if (pOut != NULL) {
		*pOut = u32Diff;
	}
	return nOv;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_overflow_u - subtract a - b with overflow detection.
 *
 * a, b: minuend / subtrahend
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a < b, else 0. Writes wrapping difference to *out when
 * out is non-NULL. No parent wires.
 */
int
gj_u32_sub_overflow_u(uint32_t a, uint32_t b, uint32_t *out)
{
	(void)NULL;
	return b4272_sub_ov(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_sub_overflow_u(uint32_t a, uint32_t b, uint32_t *out)
    __attribute__((alias("gj_u32_sub_overflow_u")));
