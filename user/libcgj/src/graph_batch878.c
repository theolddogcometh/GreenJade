/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch878: bitset no-bits-set over nwords.
 *
 * Surface (unique symbols):
 *   int gj_bitset_none(const uint64_t *words, size_t nwords);
 *     — 1 if no bit is set among nwords words, else 0.
 *       nwords==0 → 1 (vacuous). NULL words with nwords>0 → 0.
 *   int __gj_bitset_none  (alias)
 *   __libcgj_batch878_marker = "libcgj-batch878"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch878_marker[] = "libcgj-batch878";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_none — 1 if every word is zero (or nwords is 0).
 */
int
gj_bitset_none(const uint64_t *pWords, size_t cNwords)
{
	size_t i;

	if (cNwords == 0u) {
		return 1;
	}
	if (pWords == NULL) {
		return 0;
	}
	for (i = 0u; i < cNwords; i++) {
		if (pWords[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset_none(const uint64_t *pWords, size_t cNwords)
    __attribute__((alias("gj_bitset_none")));
