/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2682: UTF-8 continuation count from lead (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf8_cont_count_u(unsigned char lead);
 *     - Return the number of continuation bytes expected after lead
 *       (0..3), or -1 if lead is not a legal UTF-8 lead.
 *   int __gj_utf8_cont_count_u  (alias)
 *   __libcgj_batch2682_marker = "libcgj-batch2682"
 *
 * UTF-8 exclusive wave (2681-2690). Distinct from gj_utf8_codepoint_len
 * (batch1021, returns total length) and gj_utf8_is_cont (batch1022) —
 * unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2682_marker[] = "libcgj-batch2682";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Continuation octets needed after a legal lead (not including the lead).
 * Returns -1 for illegal leads.
 */
static int
b2682_cont_count(unsigned char u8Lead)
{
	if (u8Lead < 0x80u) {
		return 0;
	}
	/* Continuations and overlong 2-byte leads C0/C1. */
	if (u8Lead < 0xC2u) {
		return -1;
	}
	if (u8Lead < 0xE0u) {
		return 1;
	}
	if (u8Lead < 0xF0u) {
		return 2;
	}
	/* F0..F4 are legal 4-byte leads; F5..FF exceed U+10FFFF. */
	if (u8Lead <= 0xF4u) {
		return 3;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_cont_count_u - expected continuation count after lead.
 *
 * lead: first byte of a putative UTF-8 sequence.
 *
 * Returns:
 *   0  - ASCII lead (0x00..0x7F)
 *   1  - 2-byte lead (0xC2..0xDF)
 *   2  - 3-byte lead (0xE0..0xEF)
 *   3  - 4-byte lead (0xF0..0xF4)
 *  -1  - not a legal lead (continuation, C0/C1, F5..FF)
 *
 * Lead-only classification; does not inspect continuations.
 */
int
gj_utf8_cont_count_u(unsigned char u8Lead)
{
	(void)NULL;
	return b2682_cont_count(u8Lead);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_cont_count_u(unsigned char u8Lead)
    __attribute__((alias("gj_utf8_cont_count_u")));
