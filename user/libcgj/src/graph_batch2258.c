/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2258: Unicode scalar-value predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_unicode_is_scalar_cp(uint32_t u32Cp);
 *     - Return 1 if u32Cp is a Unicode scalar value: in range
 *       U+0000..U+10FFFF and not a UTF-16 surrogate (U+D800..U+DFFF).
 *       Else 0.
 *   int __gj_unicode_is_scalar_cp  (alias)
 *   __libcgj_batch2258_marker = "libcgj-batch2258"
 *
 * Distinct from gj_unicode_is_surrogate_cp (batch2257) and
 * gj_unicode_bmp_p (batch2259). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2258_marker[] = "libcgj-batch2258";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2258_is_scalar(uint32_t u32Cp)
{
	if (u32Cp > 0x10FFFFu) {
		return 0;
	}
	if (u32Cp >= 0xD800u && u32Cp <= 0xDFFFu) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unicode_is_scalar_cp - true when u32Cp is a Unicode scalar value.
 *
 * u32Cp: candidate Unicode code point
 *
 * Scalar values are U+0000..U+10FFFF excluding U+D800..U+DFFF.
 * Returns 1 if scalar, else 0. Does not call libc.
 */
int
gj_unicode_is_scalar_cp(uint32_t u32Cp)
{
	(void)NULL;
	return b2258_is_scalar(u32Cp);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unicode_is_scalar_cp(uint32_t u32Cp)
    __attribute__((alias("gj_unicode_is_scalar_cp")));
