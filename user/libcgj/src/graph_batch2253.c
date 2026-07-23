/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2253: UTF-8 2-byte lead predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_lead2(unsigned char u8B);
 *     - Return 1 if u8B is a legal 2-byte UTF-8 lead (0xC2..0xDF),
 *       else 0. C0/C1 (overlong) return 0.
 *   int __gj_utf8_is_lead2  (alias)
 *   __libcgj_batch2253_marker = "libcgj-batch2253"
 *
 * Distinct from gj_utf8_is_lead (batch1023) and lead1/3/4 siblings.
 * Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2253_marker[] = "libcgj-batch2253";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2253_is_lead2(unsigned char u8B)
{
	return (u8B >= 0xC2u && u8B <= 0xDFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead2 - true when u8B is a legal 2-byte UTF-8 lead.
 *
 * u8B: candidate octet
 *
 * Legal: 0xC2..0xDF (covers U+0080..U+07FF). C0/C1 overlong leads
 * return 0. Does not call libc.
 */
int
gj_utf8_is_lead2(unsigned char u8B)
{
	(void)NULL;
	return b2253_is_lead2(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_lead2(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead2")));
