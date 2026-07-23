/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5507: volume label length ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_vol_label_len_ok_5507(uint32_t len);
 *     - Return 1 when len is in [1, 11] (non-empty FAT-style volume
 *       label), else 0.
 *   uint32_t __gj_vol_label_len_ok_5507  (alias)
 *   __libcgj_batch5507_marker = "libcgj-batch5507"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_vol_label_len_ok_5507 surface only; no multi-def.
 * Distinct from gj_vol_label_max_len_5506 (batch5506). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5507_marker[] = "libcgj-batch5507";

/* FAT volume label field width. */
#define B5507_MAX_LEN  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5507_len_ok(uint32_t u32Len)
{
	if (u32Len == 0u) {
		return 0u;
	}
	if (u32Len > B5507_MAX_LEN) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vol_label_len_ok_5507 - true when label length is valid.
 *
 * len: proposed volume label length in bytes (excluding NUL if any)
 *
 * Returns 1 for len in [1, 11], else 0. Empty labels rejected.
 * No parent wires.
 */
uint32_t
gj_vol_label_len_ok_5507(uint32_t u32Len)
{
	(void)NULL;
	return b5507_len_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vol_label_len_ok_5507(uint32_t u32Len)
    __attribute__((alias("gj_vol_label_len_ok_5507")));
