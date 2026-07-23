/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1487: UTF-16 high-surrogate predicate.
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_high_surrogate(uint16_t u);
 *     — Returns 1 if u is a high/lead surrogate U+D800..U+DBFF, else 0.
 *   int __gj_utf16_is_high_surrogate  (alias)
 *   __libcgj_batch1487_marker = "libcgj-batch1487"
 *
 * Distinct from gj_utf16_is_surrogate (batch1486) and
 * gj_utf16_is_low_surrogate (batch1488).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1487_marker[] = "libcgj-batch1487";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_high_surrogate — true when u is U+D800..U+DBFF.
 */
int
gj_utf16_is_high_surrogate(uint16_t u)
{
	return (u >= 0xD800u && u <= 0xDBFFu) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_high_surrogate(uint16_t u)
    __attribute__((alias("gj_utf16_is_high_surrogate")));
