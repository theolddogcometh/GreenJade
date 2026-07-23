/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5508: volume label character ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vol_label_char_ok_5508(uint32_t c);
 *     - Return 1 when c is an allowed FAT-style volume label character:
 *       'A'..'Z', '0'..'9', or space (0x20). Lowercase and other
 *       punctuation → 0.
 *   uint32_t __gj_vol_label_char_ok_5508  (alias)
 *   __libcgj_batch5508_marker = "libcgj-batch5508"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_vol_label_char_ok_5508 surface only; no multi-def.
 * Distinct from gj_vol_label_len_ok_5507 (batch5507). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5508_marker[] = "libcgj-batch5508";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft FAT volume-label character set (upper alpha, digit, space).
 * Intentionally strict; does not allow lowercase or punctuation.
 */
static uint32_t
b5508_char_ok(uint32_t u32C)
{
	if (u32C == (uint32_t)' ') {
		return 1u;
	}
	if (u32C >= (uint32_t)'A' && u32C <= (uint32_t)'Z') {
		return 1u;
	}
	if (u32C >= (uint32_t)'0' && u32C <= (uint32_t)'9') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vol_label_char_ok_5508 - true when code unit is label-legal.
 *
 * c: character code (use low 8 bits for ASCII-style labels)
 *
 * Returns 1 for 'A'-'Z', '0'-'9', or space; else 0. Pure charset gate
 * for install-media volume labels. No parent wires.
 */
uint32_t
gj_vol_label_char_ok_5508(uint32_t u32C)
{
	(void)NULL;
	return b5508_char_ok(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vol_label_char_ok_5508(uint32_t u32C)
    __attribute__((alias("gj_vol_label_char_ok_5508")));
