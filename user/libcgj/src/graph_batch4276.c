/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4276: checked uint64_t mul with overflow flag (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_mul_overflow_u(uint64_t a, uint64_t b, uint64_t *out);
 *     - Compute a * b. Return 1 if the mathematical product overflows
 *       uint64_t, else 0. When out is non-NULL, write the wrapping
 *       product. out may be NULL (status-only; still return the flag).
 *       Overflow test: if (a != 0 && b > UINT64_MAX / a).
 *   int __gj_u64_mul_overflow_u  (alias)
 *   __libcgj_batch4276_marker = "libcgj-batch4276"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_mul_overflow_u64 (batch428) / gj_u64_mul_check
 * (batch646) / gj_u32_mul_overflow_u (batch4273) — unique surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4276_marker[] = "libcgj-batch4276";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Overflow iff a != 0 && b > UINT64_MAX / a.
 * Always compute wrapping product; optional store.
 */
static int
b4276_mul_ov(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	uint64_t u64Prod;
	int nOv;

	if (u64A != 0ull && u64B > (UINT64_MAX / u64A)) {
		nOv = 1;
	} else {
		nOv = 0;
	}
	u64Prod = u64A * u64B; /* wrap defined for unsigned */
	if (pOut != NULL) {
		*pOut = u64Prod;
	}
	return nOv;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_overflow_u - multiply a * b with overflow detection.
 *
 * a, b: factors
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a * b overflows uint64_t, else 0. Writes wrapping
 * product to *out when out is non-NULL. Check uses
 * (a != 0 && b > UINT64_MAX / a). No parent wires.
 */
int
gj_u64_mul_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
{
	(void)NULL;
	return b4276_mul_ov(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_mul_overflow_u(uint64_t a, uint64_t b, uint64_t *out)
    __attribute__((alias("gj_u64_mul_overflow_u")));
