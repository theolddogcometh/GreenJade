/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3339: reverse a uint32_t dense list in place
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_list_u32_rev_u(uint32_t *a, size_t n);
 *     - Reverse a[0..n) in place via two-pointer swap. NULL a or n < 2
 *       is a no-op.
 *   void __gj_list_u32_rev_u  (alias)
 *   __libcgj_batch3339_marker = "libcgj-batch3339"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_reverse_u32 (batch401) and gj_buf_rev (batch435) - unique
 * gj_list_u32_rev_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3339_marker[] = "libcgj-batch3339";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3339_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t u32Tmp;

	u32Tmp = *pA;
	*pA = *pB;
	*pB = u32Tmp;
}

/* Reverse pA[0..cN) in place. */
static void
b3339_rev(uint32_t *pA, size_t cN)
{
	size_t iLo;
	size_t iHi;

	if (pA == NULL || cN < 2u) {
		return;
	}

	iLo = 0u;
	iHi = cN - 1u;
	while (iLo < iHi) {
		b3339_swap(&pA[iLo], &pA[iHi]);
		iLo++;
		iHi--;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_u32_rev_u - reverse a dense uint32_t list in place.
 *
 * a: list base (NULL -> no-op)
 * n: element count (n < 2 -> no-op)
 *
 * Two-pointer swap from ends toward center. No parent wires.
 * Does not call libc.
 */
void
gj_list_u32_rev_u(uint32_t *a, size_t n)
{
	(void)NULL;
	b3339_rev(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_list_u32_rev_u(uint32_t *a, size_t n)
    __attribute__((alias("gj_list_u32_rev_u")));
