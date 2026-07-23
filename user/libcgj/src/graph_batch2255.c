/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2255: UTF-8 4-byte lead predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_lead4(unsigned char u8B);
 *     - Return 1 if u8B is a legal 4-byte UTF-8 lead (0xF0..0xF4),
 *       else 0. F5..F7 (above U+10FFFF) return 0.
 *   int __gj_utf8_is_lead4  (alias)
 *   __libcgj_batch2255_marker = "libcgj-batch2255"
 *
 * Distinct from gj_utf8_is_lead (batch1023) and lead1/2/3 siblings.
 * Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2255_marker[] = "libcgj-batch2255";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2255_is_lead4(unsigned char u8B)
{
	return (u8B >= 0xF0u && u8B <= 0xF4u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead4 - true when u8B is a legal 4-byte UTF-8 lead.
 *
 * u8B: candidate octet
 *
 * Legal: 0xF0..0xF4 (covers U+10000..U+10FFFF). F5..FF return 0.
 * Does not call libc.
 */
int
gj_utf8_is_lead4(unsigned char u8B)
{
	(void)NULL;
	return b2255_is_lead4(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_lead4(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead4")));
