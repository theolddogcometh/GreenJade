/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch876: bitset any-bit-set over nwords.
 *
 * Surface (unique symbols):
 *   int gj_bitset_any(const uint64_t *words, size_t nwords);
 *     — 1 if any bit is set among nwords full uint64 words, else 0.
 *       NULL words or nwords==0 → 0.
 *   int __gj_bitset_any  (alias)
 *   __libcgj_batch876_marker = "libcgj-batch876"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch876_marker[] = "libcgj-batch876";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_any — 1 if any word is nonzero.
 */
int
gj_bitset_any(const uint64_t *pWords, size_t cNwords)
{
	size_t i;

	if (pWords == NULL || cNwords == 0u) {
		return 0;
	}
	for (i = 0u; i < cNwords; i++) {
		if (pWords[i] != 0u) {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset_any(const uint64_t *pWords, size_t cNwords)
    __attribute__((alias("gj_bitset_any")));
