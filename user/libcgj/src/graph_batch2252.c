/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2252: UTF-8 1-byte lead predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_lead1(unsigned char u8B);
 *     - Return 1 if u8B is a 1-byte UTF-8 lead (ASCII 0x00..0x7F),
 *       else 0.
 *   int __gj_utf8_is_lead1  (alias)
 *   __libcgj_batch2252_marker = "libcgj-batch2252"
 *
 * Distinct from gj_utf8_is_lead (batch1023) and lead2/3/4 siblings.
 * Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2252_marker[] = "libcgj-batch2252";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2252_is_lead1(unsigned char u8B)
{
	return (u8B < 0x80u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead1 - true when u8B is an ASCII / 1-byte UTF-8 lead.
 *
 * u8B: candidate octet
 *
 * Returns 1 for 0x00..0x7F, else 0. Does not call libc.
 */
int
gj_utf8_is_lead1(unsigned char u8B)
{
	(void)NULL;
	return b2252_is_lead1(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_lead1(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead1")));
