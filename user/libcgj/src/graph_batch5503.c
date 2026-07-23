/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5503: Deck install media size ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_media_size_ok_5503(uint32_t size_mib);
 *     - Return 1 when size_mib >= 2048 (soft Deck install media
 *       minimum), else 0.
 *   uint32_t __gj_deck_media_size_ok_5503  (alias)
 *   __libcgj_batch5503_marker = "libcgj-batch5503"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_deck_media_size_ok_5503 surface only; no multi-def.
 * Distinct from gj_deck_media_min_mib_5502 (batch5502) and
 * gj_install_media_ok (batch2638). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5503_marker[] = "libcgj-batch5503";

/* Soft minimum Deck install media size (MiB); matches batch5502. */
#define B5503_MIN_MIB  2048u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5503_size_ok(uint32_t u32SizeMib)
{
	return (u32SizeMib >= B5503_MIN_MIB) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_media_size_ok_5503 - true when install media size meets min.
 *
 * size_mib: reported image/device size in mebibytes
 *
 * Returns 1 if size_mib >= 2048, else 0. Pure threshold; no I/O.
 * No parent wires.
 */
uint32_t
gj_deck_media_size_ok_5503(uint32_t u32SizeMib)
{
	(void)NULL;
	return b5503_size_ok(u32SizeMib);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_media_size_ok_5503(uint32_t u32SizeMib)
    __attribute__((alias("gj_deck_media_size_ok_5503")));
