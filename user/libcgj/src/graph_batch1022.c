/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1022: UTF-8 continuation-byte predicate.
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_cont(unsigned char b);
 *     — Return 1 if b is a UTF-8 continuation byte (10xxxxxx),
 *       else 0.
 *   int __gj_utf8_is_cont  (alias)
 *   __libcgj_batch1022_marker = "libcgj-batch1022"
 *
 * Does NOT redefine gj_utf8_codepoint_len / gj_utf8_is_lead.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1022_marker[] = "libcgj-batch1022";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_cont — true when b matches the 10xxxxxx continuation form.
 *
 * u8B: candidate octet
 *
 * Returns 1 if (u8B & 0xC0) == 0x80, else 0.
 */
int
gj_utf8_is_cont(unsigned char u8B)
{
	return ((u8B & 0xC0u) == 0x80u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_cont(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_cont")));
