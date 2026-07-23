/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch877: bitset all-bits-set over nwords.
 *
 * Surface (unique symbols):
 *   int gj_bitset_all(const uint64_t *words, size_t nwords);
 *     — 1 if every bit in every of nwords words is set, else 0.
 *       nwords==0 → 1 (vacuous). NULL words with nwords>0 → 0.
 *   int __gj_bitset_all  (alias)
 *   __libcgj_batch877_marker = "libcgj-batch877"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch877_marker[] = "libcgj-batch877";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_all — 1 if every word is UINT64_C(0xffffffffffffffff).
 */
int
gj_bitset_all(const uint64_t *pWords, size_t cNwords)
{
	size_t i;

	if (cNwords == 0u) {
		return 1;
	}
	if (pWords == NULL) {
		return 0;
	}
	for (i = 0u; i < cNwords; i++) {
		if (pWords[i] != ~((uint64_t)0)) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset_all(const uint64_t *pWords, size_t cNwords)
    __attribute__((alias("gj_bitset_all")));
