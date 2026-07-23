/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1486: UTF-16 surrogate unit predicate.
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_surrogate(uint16_t u);
 *     — Returns 1 if u is in U+D800..U+DFFF (any surrogate code unit),
 *       else 0. Surrogate units are not Unicode scalar values.
 *   int __gj_utf16_is_surrogate  (alias)
 *   __libcgj_batch1486_marker = "libcgj-batch1486"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1486_marker[] = "libcgj-batch1486";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_surrogate — true when u is any UTF-16 surrogate unit.
 */
int
gj_utf16_is_surrogate(uint16_t u)
{
	return (u >= 0xD800u && u <= 0xDFFFu) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_surrogate(uint16_t u)
    __attribute__((alias("gj_utf16_is_surrogate")));
