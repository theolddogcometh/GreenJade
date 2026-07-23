/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch692: freestanding UTF-8 code-point count (lead bytes).
 *
 * Surface (unique symbols):
 *   size_t gj_utf8_count_cp(const unsigned char *s, size_t n);
 *     — Count UTF-8 leading bytes in s[0..n): every byte that is not a
 *       continuation (bit pattern 10xxxxxx). On well-formed UTF-8 this
 *       equals the number of Unicode scalar values (code points). Does
 *       not validate sequences; bare leads / truncated tails still count
 *       as one lead each. Empty window (n == 0) returns 0 even when s is
 *       NULL. NULL s with n > 0 returns 0.
 *   size_t __gj_utf8_count_cp  (alias)
 *   __libcgj_batch692_marker = "libcgj-batch692"
 *
 * Distinct from batch78 utf8_strlen (NUL-terminated strict walk) and
 * batch259 gj_utf8_validate (whole-buffer check). Self-contained.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch692_marker[] = "libcgj-batch692";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_count_cp — count leading (non-continuation) bytes in s[0..n).
 *
 * A UTF-8 continuation byte has the form 10xxxxxx (top two bits 10).
 * Every other byte is treated as a lead / start-of-sequence byte and
 * contributes one to the count.
 *
 * Returns the count (0 if n == 0 or s is NULL).
 */
size_t
gj_utf8_count_cp(const unsigned char *s, size_t n)
{
	size_t i;
	size_t nCp;

	if (s == NULL || n == 0u) {
		return 0u;
	}

	nCp = 0u;
	for (i = 0u; i < n; i++) {
		/* Continuation: 10xxxxxx — skip. Lead / ASCII / invalid: count. */
		if ((s[i] & 0xC0u) != 0x80u) {
			nCp++;
		}
	}
	return nCp;
}

size_t __gj_utf8_count_cp(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_count_cp")));
