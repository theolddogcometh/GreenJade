/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch945: snake_case to camelCase transform into out.
 *
 * Surface (unique symbols):
 *   void gj_str_snake_to_camel(const char *in, char *out, size_t out_cap);
 *     — Convert snake_case in → camelCase out (ASCII). Underscore before
 *       a letter uppercases that letter and drops the underscore.
 *       out_cap includes NUL. NULL in/out or out_cap==0 → no-op.
 *   void __gj_str_snake_to_camel  (alias)
 *   __libcgj_batch945_marker = "libcgj-batch945"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch945_marker[] = "libcgj-batch945";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_snake_to_camel — simple snake_case → camelCase (ASCII).
 *
 * Writes at most out_cap-1 payload octets plus a terminating NUL when
 * out_cap > 0. Truncates safely if capacity is short.
 * Leading underscores are dropped (no leading upper). Consecutive
 * underscores collapse. Non-letters after '_' are copied as-is without
 * case change, and the pending-upper flag clears.
 */
void
gj_str_snake_to_camel(const char *pIn, char *pOut, size_t cOutCap)
{
	size_t iOut;
	int fUpperNext;
	const char *p;

	if (pIn == NULL || pOut == NULL || cOutCap == 0u) {
		return;
	}

	iOut = 0u;
	fUpperNext = 0;
	p = pIn;
	while (*p != '\0' && (iOut + 1u) < cOutCap) {
		unsigned char u8C = (unsigned char)*p;

		if (u8C == '_') {
			fUpperNext = 1;
			p++;
			continue;
		}
		if (fUpperNext) {
			if (u8C >= 'a' && u8C <= 'z') {
				u8C = (unsigned char)(u8C - 'a' + 'A');
			}
			fUpperNext = 0;
		}
		pOut[iOut] = (char)u8C;
		iOut++;
		p++;
	}
	pOut[iOut] = '\0';
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_str_snake_to_camel(const char *pIn, char *pOut, size_t cOutCap)
    __attribute__((alias("gj_str_snake_to_camel")));
