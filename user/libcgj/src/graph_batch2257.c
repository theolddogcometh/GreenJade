/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2257: UTF-16 surrogate code-point predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_unicode_is_surrogate_cp(uint32_t u32Cp);
 *     - Return 1 if u32Cp is a UTF-16 surrogate code point
 *       (U+D800..U+DFFF), else 0. Surrogates are not Unicode scalar
 *       values and must not appear in well-formed UTF-8.
 *   int __gj_unicode_is_surrogate_cp  (alias)
 *   __libcgj_batch2257_marker = "libcgj-batch2257"
 *
 * Unique name only; no multi-def with prior unicode surfaces.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2257_marker[] = "libcgj-batch2257";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2257_is_surrogate(uint32_t u32Cp)
{
	return (u32Cp >= 0xD800u && u32Cp <= 0xDFFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unicode_is_surrogate_cp - true when u32Cp is U+D800..U+DFFF.
 *
 * u32Cp: candidate Unicode code point
 *
 * Returns 1 for surrogates, else 0. Does not call libc.
 */
int
gj_unicode_is_surrogate_cp(uint32_t u32Cp)
{
	(void)NULL;
	return b2257_is_surrogate(u32Cp);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unicode_is_surrogate_cp(uint32_t u32Cp)
    __attribute__((alias("gj_unicode_is_surrogate_cp")));
