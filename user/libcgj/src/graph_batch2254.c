/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2254: UTF-8 3-byte lead predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_lead3(unsigned char u8B);
 *     - Return 1 if u8B is a 3-byte UTF-8 lead (0xE0..0xEF), else 0.
 *       Lead-only check; does not inspect continuations for overlong
 *       or surrogate second-byte constraints.
 *   int __gj_utf8_is_lead3  (alias)
 *   __libcgj_batch2254_marker = "libcgj-batch2254"
 *
 * Distinct from gj_utf8_is_lead (batch1023) and lead1/2/4 siblings.
 * Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2254_marker[] = "libcgj-batch2254";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2254_is_lead3(unsigned char u8B)
{
	return (u8B >= 0xE0u && u8B <= 0xEFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead3 - true when u8B is a 3-byte UTF-8 lead.
 *
 * u8B: candidate octet
 *
 * Returns 1 for 0xE0..0xEF, else 0. Does not call libc.
 */
int
gj_utf8_is_lead3(unsigned char u8B)
{
	(void)NULL;
	return b2254_is_lead3(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_lead3(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead3")));
