/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4273: checked uint32_t mul with overflow flag (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_mul_overflow_u(uint32_t a, uint32_t b, uint32_t *out);
 *     - Compute a * b. Return 1 if the mathematical product overflows
 *       uint32_t, else 0. When out is non-NULL, write the wrapping
 *       product. out may be NULL (status-only; still return the flag).
 *   int __gj_u32_mul_overflow_u  (alias)
 *   __libcgj_batch4273_marker = "libcgj-batch4273"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_mul_overflow_u64 (batch428) / gj_u64_mul_check
 * (batch646) — unique gj_u32_mul_overflow_u surface only; no multi-def.
 * No parent wires. No __int128 (overflow via UINT32_MAX / factor).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4273_marker[] = "libcgj-batch4273";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * a * b overflows iff a != 0 && b > UINT32_MAX / a.
 * Always compute wrapping product; optional store.
 */
static int
b4273_mul_ov(uint32_t u32A, uint32_t u32B, uint32_t *pOut)
{
	uint32_t u32Prod;
	int nOv;

	if (u32A != 0u && u32B > (UINT32_MAX / u32A)) {
		nOv = 1;
	} else {
		nOv = 0;
	}
	u32Prod = u32A * u32B; /* wrap defined for unsigned */
	if (pOut != NULL) {
		*pOut = u32Prod;
	}
	return nOv;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_overflow_u - multiply a * b with overflow detection.
 *
 * a, b: factors
 * out:  optional result pointer (NULL → status only)
 *
 * Returns 1 if a * b overflows uint32_t, else 0. Writes wrapping
 * product to *out when out is non-NULL. No parent wires.
 */
int
gj_u32_mul_overflow_u(uint32_t a, uint32_t b, uint32_t *out)
{
	(void)NULL;
	return b4273_mul_ov(a, b, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_mul_overflow_u(uint32_t a, uint32_t b, uint32_t *out)
    __attribute__((alias("gj_u32_mul_overflow_u")));
