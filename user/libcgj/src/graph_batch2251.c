/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2251: UTF-8 continuation-byte predicate
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_cont_byte(unsigned char u8B);
 *     - Return 1 if u8B is a UTF-8 continuation byte (10xxxxxx),
 *       else 0.
 *   int __gj_utf8_is_cont_byte  (alias)
 *   __libcgj_batch2251_marker = "libcgj-batch2251"
 *
 * Distinct from gj_utf8_is_cont (batch1022). Unique name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2251_marker[] = "libcgj-batch2251";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2251_is_cont(unsigned char u8B)
{
	return ((u8B & 0xC0u) == 0x80u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_cont_byte - true when u8B matches the 10xxxxxx form.
 *
 * u8B: candidate octet
 *
 * Returns 1 if continuation, else 0. Does not call libc.
 */
int
gj_utf8_is_cont_byte(unsigned char u8B)
{
	(void)NULL;
	return b2251_is_cont(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_cont_byte(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_cont_byte")));
