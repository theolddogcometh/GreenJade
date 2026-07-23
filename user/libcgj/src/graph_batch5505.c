/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5505: Deck install media checksum equality.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_media_checksum_eq_5505(uint32_t got, uint32_t expect);
 *     - Return 1 when got == expect, else 0. Soft compare for media
 *       integrity digests from gj_deck_media_checksum_5504.
 *   uint32_t __gj_deck_media_checksum_eq_5505  (alias)
 *   __libcgj_batch5505_marker = "libcgj-batch5505"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510: Deck
 * install media verification / checksum / volume labels / ESP size).
 * Unique gj_deck_media_checksum_eq_5505 surface only; no multi-def.
 * Distinct from gj_deck_media_checksum_5504 (batch5504). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5505_marker[] = "libcgj-batch5505";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5505_checksum_eq(uint32_t u32Got, uint32_t u32Expect)
{
	return (u32Got == u32Expect) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_media_checksum_eq_5505 - true when digests match.
 *
 * got:    computed checksum (e.g. from gj_deck_media_checksum_5504)
 * expect: expected published digest
 *
 * Returns 1 on exact equality, else 0. Pure compare; no I/O.
 * No parent wires.
 */
uint32_t
gj_deck_media_checksum_eq_5505(uint32_t u32Got, uint32_t u32Expect)
{
	(void)NULL;
	return b5505_checksum_eq(u32Got, u32Expect);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_media_checksum_eq_5505(uint32_t u32Got, uint32_t u32Expect)
    __attribute__((alias("gj_deck_media_checksum_eq_5505")));
