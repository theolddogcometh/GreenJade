/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3581: UTF-8 sequence length from lead byte (_u).
 *
 * Surface (unique symbols):
 *   unsigned int gj_utf8_lead_bytes_u(unsigned char lead);
 *     - Return expected total sequence length (1..4) from a UTF-8 lead
 *       octet, or 0 if lead cannot start a well-formed sequence
 *       (continuation, C0/C1 overlong, F5..FF). Lead-only; does not
 *       inspect continuation bytes.
 *   unsigned int __gj_utf8_lead_bytes_u  (alias)
 *   __libcgj_batch3581_marker = "libcgj-batch3581"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590: utf8_lead_bytes_u,
 * utf8_is_valid_seq_u, utf16_is_surrogate_u, utf16_is_high_surr_u,
 * utf16_is_low_surr_u, ascii_fold_char_u, ascii_eq_ci_char_u,
 * ws_is_space_u, ws_skip_u, batch_id_3590). Distinct from
 * gj_utf8_codepoint_len (batch1021, returns -1 on bad lead) —
 * unique 0-on-invalid unsigned surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3581_marker[] = "libcgj-batch3581";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classify lead: 1..4 or 0. */
static unsigned int
b3581_lead_bytes(unsigned char u8Lead)
{
	if (u8Lead < 0x80u) {
		return 1u;
	}
	/* Continuations 80..BF and overlong 2-byte leads C0/C1. */
	if (u8Lead < 0xC2u) {
		return 0u;
	}
	if (u8Lead < 0xE0u) {
		return 2u;
	}
	if (u8Lead < 0xF0u) {
		return 3u;
	}
	/* F0..F4 legal 4-byte leads; F5..FF exceed U+10FFFF. */
	if (u8Lead <= 0xF4u) {
		return 4u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_lead_bytes_u - expected UTF-8 sequence length from lead byte.
 *
 * lead: first octet of a putative UTF-8 sequence
 *
 * Returns:
 *   1  - ASCII lead (0x00..0x7F)
 *   2  - 2-byte lead (0xC2..0xDF)
 *   3  - 3-byte lead (0xE0..0xEF)
 *   4  - 4-byte lead (0xF0..0xF4)
 *   0  - not a legal lead
 */
unsigned int
gj_utf8_lead_bytes_u(unsigned char u8Lead)
{
	(void)NULL;
	return b3581_lead_bytes(u8Lead);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned int __gj_utf8_lead_bytes_u(unsigned char u8Lead)
    __attribute__((alias("gj_utf8_lead_bytes_u")));
