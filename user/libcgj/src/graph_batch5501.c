/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5501: Deck install media ISO9660 magic check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_media_magic_ok_5501(const uint8_t *p, size_t n);
 *     - Return 1 when buffer holds a Primary Volume Descriptor-shaped
 *       ISO9660 header: type byte 0x01 and "CD001" at bytes 1..5.
 *       Requires n >= 6. Else 0.
 *   uint32_t __gj_deck_media_magic_ok_5501  (alias)
 *   __libcgj_batch5501_marker = "libcgj-batch5501"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification — deck_media_magic_ok_5501,
 * deck_media_min_mib_5502, deck_media_size_ok_5503,
 * deck_media_checksum_5504, deck_media_checksum_eq_5505,
 * vol_label_max_len_5506, vol_label_len_ok_5507, vol_label_char_ok_5508,
 * esp_size_min_mib_5509, esp_size_ok_5510). Unique gj_*_5501 surface
 * only; no multi-def. Distinct from gj_install_media_ok (batch2638).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5501_marker[] = "libcgj-batch5501";

/* ISO9660 Primary Volume Descriptor type code. */
#define B5501_PVD_TYPE   0x01u
/* Minimum bytes needed for type + "CD001". */
#define B5501_MIN_LEN    6u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PVD magic probe: p[0]==1 and p[1..5]=='C','D','0','0','1'.
 * Does not walk the full volume descriptor set.
 */
static uint32_t
b5501_magic_ok(const uint8_t *p, size_t n)
{
	if (p == NULL || n < B5501_MIN_LEN) {
		return 0u;
	}
	if (p[0] != (uint8_t)B5501_PVD_TYPE) {
		return 0u;
	}
	if (p[1] != (uint8_t)'C' || p[2] != (uint8_t)'D' ||
	    p[3] != (uint8_t)'0' || p[4] != (uint8_t)'0' ||
	    p[5] != (uint8_t)'1') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_media_magic_ok_5501 - ISO9660 PVD magic soft check.
 *
 * p: buffer start (may be NULL → 0)
 * n: buffer length in bytes
 *
 * Returns 1 when the first six bytes match a Primary Volume Descriptor
 * type+identifier, else 0. Pure data; no I/O. No parent wires.
 */
uint32_t
gj_deck_media_magic_ok_5501(const uint8_t *p, size_t n)
{
	(void)NULL;
	return b5501_magic_ok(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_media_magic_ok_5501(const uint8_t *p, size_t n)
    __attribute__((alias("gj_deck_media_magic_ok_5501")));
