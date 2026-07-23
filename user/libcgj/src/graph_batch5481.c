/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5481: Deck Top 50 title-catalog size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_top50_n_5481(void);
 *     - Returns 50 (soft catalog size for Deck Top 50 game titles
 *       tracked by this exclusive pure-data wave). Stub integer only.
 *   uint32_t __gj_deck_top50_n_5481  (alias)
 *   __libcgj_batch5481_marker = "libcgj-batch5481"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_deck_top50_n_5481 surface only; no
 * multi-def. Distinct from prior deck/appid predicates. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5481_marker[] = "libcgj-batch5481";

/* Soft Deck Top 50 catalog size. */
#define B5481_TOP50_N  50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5481_top50_n(void)
{
	return B5481_TOP50_N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_top50_n_5481 - report Deck Top 50 catalog size stub.
 *
 * Always returns 50. Link-time presence tags the Top 50 readiness wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_deck_top50_n_5481(void)
{
	(void)NULL;
	return b5481_top50_n();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_top50_n_5481(void)
    __attribute__((alias("gj_deck_top50_n_5481")));
