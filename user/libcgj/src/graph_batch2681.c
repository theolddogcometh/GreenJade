/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2681: UTF-8 legal lead-byte predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf8_valid_lead_u(unsigned char b);
 *     - Return 1 if b can start a well-formed UTF-8 sequence
 *       (ASCII 0x00..0x7F or legal multi-byte lead C2..F4), else 0.
 *   int __gj_utf8_valid_lead_u  (alias)
 *   __libcgj_batch2681_marker = "libcgj-batch2681"
 *
 * UTF-8 exclusive wave (2681-2690). Distinct from gj_utf8_is_lead
 * (batch1023) and lead1/2/3/4 siblings — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2681_marker[] = "libcgj-batch2681";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8B is a legal UTF-8 sequence lead (ASCII or C2..F4). */
static int
b2681_valid_lead(unsigned char u8B)
{
	if (u8B < 0x80u) {
		return 1;
	}
	if (u8B >= 0xC2u && u8B <= 0xF4u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_valid_lead_u - true when b is a legal UTF-8 sequence lead.
 *
 * b: candidate octet
 *
 * Legal leads:
 *   0x00..0x7F  - ASCII (1-byte)
 *   0xC2..0xDF  - 2-byte
 *   0xE0..0xEF  - 3-byte
 *   0xF0..0xF4  - 4-byte
 * Continuations (80..BF), C0/C1, and F5..FF return 0.
 */
int
gj_utf8_valid_lead_u(unsigned char u8B)
{
	(void)NULL;
	return b2681_valid_lead(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_valid_lead_u(unsigned char u8B)
    __attribute__((alias("gj_utf8_valid_lead_u")));
