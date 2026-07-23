/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5502: Deck install media minimum size (MiB).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_media_min_mib_5502(void);
 *     - Returns 2048 (soft minimum install-image size in mebibytes for
 *       Deck continuum media verification).
 *   uint32_t __gj_deck_media_min_mib_5502  (alias)
 *   __libcgj_batch5502_marker = "libcgj-batch5502"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_deck_media_min_mib_5502 surface only; no multi-def.
 * Distinct from gj_install_media_ok (batch2638). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5502_marker[] = "libcgj-batch5502";

/* Soft minimum Deck install media size (MiB). */
#define B5502_MIN_MIB  2048u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5502_min_mib(void)
{
	return B5502_MIN_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_media_min_mib_5502 - report soft minimum install media size.
 *
 * Always returns 2048 (mebibytes). Compile-time product-path constant
 * for post-milestone 5500 Deck install media verification. Does not
 * probe storage. No parent wires.
 */
uint32_t
gj_deck_media_min_mib_5502(void)
{
	(void)NULL;
	return b5502_min_mib();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_media_min_mib_5502(void)
    __attribute__((alias("gj_deck_media_min_mib_5502")));
