/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2898: paired static smoke soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_smoke_pair_u(int code_a, int code_b);
 *     - Return 1 if both static smoke codes are strict green PASS
 *       (exactly 0), else 0. Soft pure-data gate for paired static
 *       smoke lamps. Unsigned 0/1 form.
 *   uint32_t __gj_product_smoke_pair_u  (alias)
 *   __libcgj_batch2898_marker = "libcgj-batch2898"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_product_smoke_pair_u surface only; no multi-def. Distinct from
 * gj_smoke_green_pair_p (batch2693 int), gj_smoke_both_pass_p
 * (batch2594), and gj_product_dyn_pair_u (batch2899 soft-band).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2898_marker[] = "libcgj-batch2898";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if both codes are strict green PASS (exactly 0). */
static uint32_t
b2898_smoke_pair(int nCodeA, int nCodeB)
{
	if (nCodeA != 0) {
		return 0u;
	}
	if (nCodeB != 0) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_smoke_pair_u - 1 iff both static smoke codes are green.
 *
 * code_a: first static smoke result
 * code_b: second static smoke result
 *
 * Green PASS is exactly 0 for both. Pair with gj_product_dyn_pair_u
 * for the dyn soft-band counterpart. Does not call libc.
 */
uint32_t
gj_product_smoke_pair_u(int nCodeA, int nCodeB)
{
	(void)NULL;
	return b2898_smoke_pair(nCodeA, nCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_smoke_pair_u(int nCodeA, int nCodeB)
    __attribute__((alias("gj_product_smoke_pair_u")));
