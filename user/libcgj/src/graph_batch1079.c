/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1079: format boolean as ASCII token.
 *
 * Surface (unique symbols):
 *   int gj_format_bool(int v, char *out, size_t cap);
 *     — Write "true" if v != 0, else "false", as a NUL-terminated
 *       string. Returns length excluding NUL on success, or -1 if
 *       out == NULL, cap == 0, or the token does not fit.
 *   int __gj_format_bool  (alias)
 *   __libcgj_batch1079_marker = "libcgj-batch1079"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1079_marker[] = "libcgj-batch1079";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_bool — write "true" / "false" for fVal into pOut.
 *
 * fVal: non-zero → "true" (4 chars); zero → "false" (5 chars)
 */
int
gj_format_bool(int fVal, char *pOut, size_t cbCap)
{
	const char *sz;
	size_t cb;
	size_t i;

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

	for (i = 0u; i < cb; i++) {
		pOut[i] = sz[i];
	}
	pOut[cb] = '\0';
	return (int)cb;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_bool(int fVal, char *pOut, size_t cbCap)
    __attribute__((alias("gj_format_bool")));
