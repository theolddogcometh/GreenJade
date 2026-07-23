/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch434: freestanding byte-buffer substring search.
 *
 * Surface (unique symbols):
 *   size_t gj_buf_find(const uint8_t *hay, size_t nh,
 *                      const uint8_t *needle, size_t nn);
 *     — Return the lowest index i in [0, nh-nn] such that
 *       hay[i..i+nn) equals needle[0..nn). Empty needle (nn == 0) matches
 *       at index 0 and returns 0. If not found, returns nh. NULL hay with
 *       nh > 0, or NULL needle with nn > 0, yields nh (not found).
 *   size_t __gj_buf_find  (alias)
 *   __libcgj_batch434_marker = "libcgj-batch434"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch434_marker[] = "libcgj-batch434";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_find — first index of needle inside hay, or nh if absent.
 *
 * hay / nh: haystack base + length
 * needle / nn: pattern base + length
 *
 * Returns match index, or nh when not found / invalid non-empty NULL base.
 */
size_t
gj_buf_find(const uint8_t *pHay, size_t cbNh,
            const uint8_t *pNeedle, size_t cbNn)
{
	size_t i;
	size_t j;
	size_t cbLimit;

	if (cbNn == 0u) {
		return 0u;
	}

	if (pHay == NULL || pNeedle == NULL) {
		return cbNh;
	}

	if (cbNn > cbNh) {
		return cbNh;
	}

	cbLimit = cbNh - cbNn;
	for (i = 0u; i <= cbLimit; i++) {
		for (j = 0u; j < cbNn; j++) {
			if (pHay[i + j] != pNeedle[j]) {
				break;
			}
		}
		if (j == cbNn) {
			return i;
		}
	}

	return cbNh;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_buf_find(const uint8_t *pHay, size_t cbNh,
                     const uint8_t *pNeedle, size_t cbNn)
    __attribute__((alias("gj_buf_find")));
