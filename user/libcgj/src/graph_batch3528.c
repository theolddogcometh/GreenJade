/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3528: ascending shell sort of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   void gj_sort_u64_shell_u(uint64_t *a, size_t n);
 *     - Sort a[0..n) ascending (nondecreasing) in place via shell
 *       sort (gap sequence n/2, n/4, ..., 1). a == NULL or n < 2 →
 *       no-op. Not stable.
 *   void __gj_sort_u64_shell_u  (alias)
 *   __libcgj_batch3528_marker = "libcgj-batch3528"
 *
 * Exclusive sort algorithms wave (3521-3530). Companion to
 * gj_sort_u32_shell_u (batch3527). Unique _u name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3528_marker[] = "libcgj-batch3528";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Shell sort a[0..n) with classic n/2 gap sequence.
 * pA non-NULL, cN >= 2. Not stable.
 */
static void
b3528_shell(uint64_t *pA, size_t cN)
{
	size_t cGap;

	for (cGap = cN / 2u; cGap > 0u; cGap /= 2u) {
		size_t i;

		for (i = cGap; i < cN; i++) {
			uint64_t uKey = pA[i];
			size_t j = i;

			while (j >= cGap && pA[j - cGap] > uKey) {
				pA[j] = pA[j - cGap];
				j -= cGap;
			}
			pA[j] = uKey;
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_u64_shell_u - ascending shell sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   not stable for equal keys
 */
void
gj_sort_u64_shell_u(uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b3528_shell(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_u64_shell_u(uint64_t *pA, size_t cN)
    __attribute__((alias("gj_sort_u64_shell_u")));
