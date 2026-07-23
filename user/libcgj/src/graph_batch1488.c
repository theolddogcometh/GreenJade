/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1488: UTF-16 low-surrogate predicate.
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_low_surrogate(uint16_t u);
 *     — Returns 1 if u is a low/trail surrogate U+DC00..U+DFFF, else 0.
 *   int __gj_utf16_is_low_surrogate  (alias)
 *   __libcgj_batch1488_marker = "libcgj-batch1488"
 *
 * Distinct from gj_utf16_is_surrogate (batch1486) and
 * gj_utf16_is_high_surrogate (batch1487).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1488_marker[] = "libcgj-batch1488";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_low_surrogate — true when u is U+DC00..U+DFFF.
 */
int
gj_utf16_is_low_surrogate(uint16_t u)
{
	return (u >= 0xDC00u && u <= 0xDFFFu) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_low_surrogate(uint16_t u)
    __attribute__((alias("gj_utf16_is_low_surrogate")));
