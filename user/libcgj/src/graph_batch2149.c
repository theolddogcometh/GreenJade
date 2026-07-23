/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2149: format boolean as lowercase ASCII token.
 *
 * Surface (unique symbols):
 *   int gj_format_bool_lower(int v, char *out, size_t cap);
 *     - Write "true" if v != 0, else "false", as a NUL-terminated
 *       lowercase string. Returns length excluding NUL on success, or
 *       -1 if out == NULL, cap == 0, or the token does not fit.
 *   int __gj_format_bool_lower  (alias)
 *   __libcgj_batch2149_marker = "libcgj-batch2149"
 *
 * Distinct from gj_format_bool (batch1079) - unique surface for the
 * post-2140 parse exclusive wave (2141-2150). Same lowercase tokens
 * paired with gj_parse_bool_strict (batch2141).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2149_marker[] = "libcgj-batch2149";

/* ---- helpers ----------------------------------------------------------- */

/* Copy cb bytes from pSrc to pDst (no overlap assumed; freestanding). */
static void
b2149_copy(char *pDst, const char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_bool_lower - write lowercase "true" / "false" for fVal.
 *
 * fVal: non-zero -> "true" (4 chars); zero -> "false" (5 chars)
 *
 * Edge cases:
 *   out == NULL or cap == 0 -> -1
 *   cap too small for token + NUL -> -1
 * Success: length excluding NUL.
 */
int
gj_format_bool_lower(int fVal, char *pOut, size_t cbCap)
{
	const char *sz;
	size_t cb;

	(void)NULL;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	if (fVal != 0) {
		sz = "true";
		cb = 4u;
	} else {
		sz = "false";
		cb = 5u;
	}

	if (cbCap < cb + 1u) {
		return -1;
	}

	b2149_copy(pOut, sz, cb);
	pOut[cb] = '\0';
	return (int)cb;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_bool_lower(int fVal, char *pOut, size_t cbCap)
    __attribute__((alias("gj_format_bool_lower")));
