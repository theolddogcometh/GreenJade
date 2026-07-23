/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5506: volume label maximum length (FAT-style).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vol_label_max_len_5506(void);
 *     - Returns 11 (classic FAT volume label field width in bytes).
 *   uint32_t __gj_vol_label_max_len_5506  (alias)
 *   __libcgj_batch5506_marker = "libcgj-batch5506"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_vol_label_max_len_5506 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5506_marker[] = "libcgj-batch5506";

/* FAT volume label field width. */
#define B5506_MAX_LEN  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5506_max_len(void)
{
	return B5506_MAX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vol_label_max_len_5506 - report max FAT-style volume label length.
 *
 * Always returns 11. Soft constant for install-media volume label
 * validation in the Deck continuum. No parent wires.
 */
uint32_t
gj_vol_label_max_len_5506(void)
{
	(void)NULL;
	return b5506_max_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vol_label_max_len_5506(void)
    __attribute__((alias("gj_vol_label_max_len_5506")));
