/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5504: Deck install media rolling checksum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_media_checksum_5504(const void *data, size_t len);
 *     - 32-bit FNV-1a style rolling checksum over len bytes. Empty or
 *       NULL with len==0 → basis (0x811c9dc5). NULL with len>0 → 0.
 *   uint32_t __gj_deck_media_checksum_5504  (alias)
 *   __libcgj_batch5504_marker = "libcgj-batch5504"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_deck_media_checksum_5504 surface only; no multi-def.
 * Distinct from gj_ip_checksum (batch124) and gj_tar_checksum_ok
 * (batch101). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5504_marker[] = "libcgj-batch5504";

/* FNV-1a 32-bit offset basis and prime (public domain constants). */
#define B5504_FNV_BASIS  0x811c9dc5u
#define B5504_FNV_PRIME  0x01000193u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * FNV-1a 32: hash = basis; for each byte: hash ^= byte; hash *= prime.
 * Integer-only; wraps naturally in uint32_t.
 */
static uint32_t
b5504_checksum(const void *pvData, size_t nLen)
{
	const uint8_t *p;
	size_t i;
	uint32_t u32Hash;

	if (nLen == 0u) {
		return B5504_FNV_BASIS;
	}
	if (pvData == NULL) {
		return 0u;
	}
	p = (const uint8_t *)pvData;
	u32Hash = B5504_FNV_BASIS;
	for (i = 0u; i < nLen; i++) {
		u32Hash ^= (uint32_t)p[i];
		u32Hash *= B5504_FNV_PRIME;
	}
	return u32Hash;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_media_checksum_5504 - FNV-1a 32-bit checksum of a buffer.
 *
 * data: bytes to hash (NULL only legal when len == 0)
 * len:  byte count
 *
 * Returns the rolling 32-bit digest. Soft media-integrity helper for
 * Deck install continuum; not a cryptographic hash. No parent wires.
 */
uint32_t
gj_deck_media_checksum_5504(const void *pvData, size_t nLen)
{
	(void)NULL;
	return b5504_checksum(pvData, nLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_media_checksum_5504(const void *pvData, size_t nLen)
    __attribute__((alias("gj_deck_media_checksum_5504")));
