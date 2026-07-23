/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1638: word-wise OR-NOT over uint32 bitsets.
 *
 * Surface (unique symbols):
 *   void gj_bitset_u32_or_not(uint32_t *dst, const uint32_t *a,
 *                             const uint32_t *b, size_t nwords);
 *     — For each i in [0, nwords): dst[i] = a[i] | ~b[i].
 *       NULL dst/a/b or nwords == 0 → no-op.
 *   void __gj_bitset_u32_or_not  (alias)
 *   __libcgj_batch1638_marker = "libcgj-batch1638"
 *
 * Distinct from gj_bitset_u32_and_not (batch1637) — unique OR-NOT
 * three-operand u32 surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1638_marker[] = "libcgj-batch1638";

/* ---- freestanding helpers ---------------------------------------------- */

/* dst[i] = a[i] | ~b[i] for cNwords words (caller validated non-NULL). */
static void
b1638_or_not(uint32_t *pDst, const uint32_t *pA, const uint32_t *pB,
    size_t cNwords)
{
	size_t i;

	for (i = 0u; i < cNwords; i++) {
		pDst[i] = pA[i] | ~pB[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_u32_or_not — dense u32 bitset OR-NOT (a union complement of b).
 *
 * dst: destination word array (may alias a or b).
 * a: left operand words.
 * b: right operand words (complemented before OR).
 * nwords: number of uint32_t words.
 *
 * NULL any pointer or nwords == 0 → no-op.
 */
void
gj_bitset_u32_or_not(uint32_t *pDst, const uint32_t *pA, const uint32_t *pB,
    size_t cNwords)
{
	if (pDst == NULL || pA == NULL || pB == NULL || cNwords == 0u) {
		return;
	}
	b1638_or_not(pDst, pA, pB, cNwords);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitset_u32_or_not(uint32_t *pDst, const uint32_t *pA,
    const uint32_t *pB, size_t cNwords)
    __attribute__((alias("gj_bitset_u32_or_not")));
