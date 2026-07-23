/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2256: UTF-8 sequence length from lead byte
 * (post-2250 unicode exclusive lightweight surface).
 *
 * Surface (unique symbols):
 *   int gj_utf8_seq_len_from_lead(unsigned char u8Lead);
 *     - Return expected sequence length (1..4) from a UTF-8 lead
 *       byte, or 0 if u8Lead cannot start a well-formed sequence
 *       (continuation, C0/C1 overlong, F5..FF, etc.).
 *   int __gj_utf8_seq_len_from_lead  (alias)
 *   __libcgj_batch2256_marker = "libcgj-batch2256"
 *
 * Distinct from gj_utf8_codepoint_len (batch1021) which returns -1 on
 * invalid lead. This surface returns 0 on invalid. Unique name only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2256_marker[] = "libcgj-batch2256";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2256_seq_len(unsigned char u8Lead)
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
 * gj_utf8_seq_len_from_lead - expected UTF-8 sequence length from lead.
 *
 * u8Lead: first byte of a putative UTF-8 sequence.
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
gj_utf8_seq_len_from_lead(unsigned char u8Lead)
{
	(void)NULL;
	return b2256_seq_len(u8Lead);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_seq_len_from_lead(unsigned char u8Lead)
    __attribute__((alias("gj_utf8_seq_len_from_lead")));
