/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2683: UTF-8 codepoint total bytes from lead (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf8_codepoint_bytes_u(unsigned char lead);
 *     - Return expected sequence length in bytes (1..4) from a UTF-8
 *       lead byte, or 0 if lead cannot start a well-formed sequence.
 *   int __gj_utf8_codepoint_bytes_u  (alias)
 *   __libcgj_batch2683_marker = "libcgj-batch2683"
 *
 * UTF-8 exclusive wave (2681-2690). Distinct from gj_utf8_codepoint_len
 * (batch1021, -1 on invalid), gj_utf8_seq_len_from_lead (batch2256),
 * and gj_utf8_cont_count_u (batch2682, continuations only) — unique _u
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2683_marker[] = "libcgj-batch2683";

/* ---- freestanding helpers ---------------------------------------------- */

/* Total sequence length 1..4 from legal lead; 0 if illegal. */
static int
b2683_bytes(unsigned char u8Lead)
{
	if (u8Lead < 0x80u) {
		return 1;
	}
	/* Continuations and overlong 2-byte leads C0/C1. */
	if (u8Lead < 0xC2u) {
		return 0;
	}
	if (u8Lead < 0xE0u) {
		return 2;
	}
	if (u8Lead < 0xF0u) {
		return 3;
	}
	/* F0..F4 are legal 4-byte leads; F5..FF exceed U+10FFFF. */
	if (u8Lead <= 0xF4u) {
		return 4;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_codepoint_bytes_u - expected UTF-8 sequence length from lead.
 *
 * lead: first byte of a putative UTF-8 sequence.
 *
 * Returns:
 *   1  - ASCII lead (0x00..0x7F)
 *   2  - 2-byte lead (0xC2..0xDF)
 *   3  - 3-byte lead (0xE0..0xEF)
 *   4  - 4-byte lead (0xF0..0xF4)
 *   0  - not a legal lead (continuation, C0/C1, F5..FF)
 *
 * Lead-only classification; does not inspect continuations.
 */
int
gj_utf8_codepoint_bytes_u(unsigned char u8Lead)
{
	(void)NULL;
	return b2683_bytes(u8Lead);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_codepoint_bytes_u(unsigned char u8Lead)
    __attribute__((alias("gj_utf8_codepoint_bytes_u")));
