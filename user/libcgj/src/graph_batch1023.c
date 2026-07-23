/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1023: UTF-8 lead-byte predicate.
 *
 * Surface (unique symbols):
 *   int gj_utf8_is_lead(unsigned char b);
 *     — Return 1 if b can start a well-formed UTF-8 sequence
 *       (ASCII or legal multi-byte lead C2..F4), else 0.
 *   int __gj_utf8_is_lead  (alias)
 *   __libcgj_batch1023_marker = "libcgj-batch1023"
 *
 * Does NOT redefine gj_utf8_is_cont / gj_utf8_codepoint_len.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1023_marker[] = "libcgj-batch1023";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_is_lead — true when b is a legal UTF-8 sequence lead.
 *
 * u8B: candidate octet
 *
 * Legal leads:
 *   0x00..0x7F  — ASCII (1-byte)
 *   0xC2..0xDF  — 2-byte
 *   0xE0..0xEF  — 3-byte
 *   0xF0..0xF4  — 4-byte
 * Continuations (80..BF), C0/C1, and F5..FF return 0.
 */
int
gj_utf8_is_lead(unsigned char u8B)
{
	if (u8B < 0x80u) {
		return 1;
	}
	if (u8B >= 0xC2u && u8B <= 0xF4u) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_is_lead(unsigned char u8B)
    __attribute__((alias("gj_utf8_is_lead")));
