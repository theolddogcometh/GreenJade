/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2553: freestanding in-place ascending Shell sort
 * over a bounded uint32_t array (sort exclusive wave 2551-2560).
 *
 * Surface (unique symbols):
 *   void gj_u32_shell_sort_asc(uint32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via Shell sort
 *       (gap sequence: cN/2, cN/4, ..., 1). NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_u32_shell_sort_asc  (alias)
 *   __libcgj_batch2553_marker = "libcgj-batch2553"
 *
 * Distinct from insertion sorts (batch669/2231/2551/2552) and cocktail
 * (batch2554). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2553_marker[] = "libcgj-batch2553";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2553_shell_sort(uint32_t *pA, size_t cN)
{
	size_t cGap;
	size_t i;

	for (cGap = cN / 2u; cGap > 0u; cGap /= 2u) {
		for (i = cGap; i < cN; i++) {
			uint32_t u32Key = pA[i];
			size_t j = i;

			while (j >= cGap && pA[j - cGap] > u32Key) {
				pA[j] = pA[j - cGap];
				j -= cGap;
			}
			pA[j] = u32Key;
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shell_sort_asc - in-place ascending Shell sort of cN words.
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Uses diminishing gaps cN/2, cN/4, ..., 1 with insertion within each
 * gap. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_shell_sort_asc(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2553_shell_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_shell_sort_asc(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_shell_sort_asc")));
