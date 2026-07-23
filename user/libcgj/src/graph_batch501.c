/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch501: freestanding UTF-16LE unit count (bounded).
 *
 * Surface (unique symbols):
 *   size_t gj_utf16le_units(const uint16_t *s, size_t max_units);
 *     — Count UTF-16LE code units in s until a 0 unit or max_units.
 *       Does not decode surrogates; units only (strnlen for uint16_t).
 *   size_t __gj_utf16le_units  (alias)
 *   __libcgj_batch501_marker = "libcgj-batch501"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch501_marker[] = "libcgj-batch501";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16le_units — bounded UTF-16LE code-unit length (like strnlen).
 *
 * On LE hosts, uint16_t values are already UTF-16LE wire order; a unit
 * of 0 is the terminator in either byte order. Count units only — no
 * surrogate pair decoding.
 *
 * Returns:
 *   0           if s is NULL or max_units is 0
 *   n < max     if s[n] == 0 is first zero (units before terminator)
 *   max_units   if no zero in the first max_units units
 */
size_t
gj_utf16le_units(const uint16_t *s, size_t max_units)
{
	size_t i;

	if (s == NULL) {
		return 0;
	}

	for (i = 0; i < max_units; i++) {
		if (s[i] == 0) {
			return i;
		}
	}
	return max_units;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_utf16le_units(const uint16_t *s, size_t max_units)
    __attribute__((alias("gj_utf16le_units")));
