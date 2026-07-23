/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2259: Unicode BMP (plane 0) predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_unicode_bmp_p(uint32_t u32Cp);
 *     - Return 1 if u32Cp is in the Basic Multilingual Plane
 *       (U+0000..U+FFFF), else 0. Surrogate code points in that range
 *       still return 1 (plane membership only; use scalar_cp for
 *       scalar-value checks).
 *   int __gj_unicode_bmp_p  (alias)
 *   __libcgj_batch2259_marker = "libcgj-batch2259"
 *
 * Distinct from gj_unicode_is_scalar_cp (batch2258). Unique name only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2259_marker[] = "libcgj-batch2259";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2259_is_bmp(uint32_t u32Cp)
{
	return (u32Cp <= 0xFFFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unicode_bmp_p - true when u32Cp is in the BMP (U+0000..U+FFFF).
 *
 * u32Cp: candidate Unicode code point
 *
 * Plane-0 membership only. Returns 1 if <= U+FFFF, else 0.
 * Does not call libc.
 */
int
gj_unicode_bmp_p(uint32_t u32Cp)
{
	(void)NULL;
	return b2259_is_bmp(u32Cp);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unicode_bmp_p(uint32_t u32Cp)
    __attribute__((alias("gj_unicode_bmp_p")));
