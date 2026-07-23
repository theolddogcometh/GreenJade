/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1021: UTF-8 expected codepoint length from lead byte.
 *
 * Surface (unique symbols):
 *   int gj_utf8_codepoint_len(unsigned char lead);
 *     — Return expected sequence length (1..4) from a UTF-8 lead
 *       byte, or -1 if lead cannot start a well-formed sequence
 *       (continuation, C0/C1 overlong, F5..FF, etc.).
 *   int __gj_utf8_codepoint_len  (alias)
 *   __libcgj_batch1021_marker = "libcgj-batch1021"
 *
 * Does NOT redefine gj_utf8_decode1 / gj_utf8_validate / gj_utf8_is_ascii.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1021_marker[] = "libcgj-batch1021";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_codepoint_len — expected UTF-8 sequence length from lead byte.
 *
 * u8Lead: first byte of a putative UTF-8 sequence.
 *
 * Returns:
 *   1  — ASCII lead (0x00..0x7F)
 *   2  — 2-byte lead (0xC2..0xDF)
 *   3  — 3-byte lead (0xE0..0xEF)
 *   4  — 4-byte lead (0xF0..0xF4)
 *  -1  — not a legal lead (continuation, C0/C1, F5..FF)
 *
 * This only classifies the lead byte; it does not inspect continuations.
 */
int
gj_utf8_codepoint_len(unsigned char u8Lead)
{
	if (u8Lead < 0x80u) {
		return 1;
	}
	/* Continuations and overlong 2-byte leads C0/C1. */
	if (u8Lead < 0xC2u) {
		return -1;
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
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_codepoint_len(unsigned char u8Lead)
    __attribute__((alias("gj_utf8_codepoint_len")));
