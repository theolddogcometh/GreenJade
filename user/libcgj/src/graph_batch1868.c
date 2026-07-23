/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1868: Steam Deck native resolution hint.
 *
 * Surface (unique symbols):
 *   int gj_deck_res_is_native_hint(unsigned w, unsigned h);
 *     — Return 1 if (w, h) is the Steam Deck LCD native resolution
 *       1280×800, else 0. Soft display-mode hint only.
 *   int __gj_deck_res_is_native_hint  (alias)
 *   __libcgj_batch1868_marker = "libcgj-batch1868"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1868_marker[] = "libcgj-batch1868";

/* Steam Deck LCD native panel size. */
#define B1868_DECK_W  1280u
#define B1868_DECK_H  800u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1868_is_native(unsigned uW, unsigned uH)
{
	return (uW == B1868_DECK_W && uH == B1868_DECK_H) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_res_is_native_hint — 1 iff resolution is 1280×800.
 *
 * w, h: pixel width and height. Orientation-sensitive (800×1280 → 0).
 */
int
gj_deck_res_is_native_hint(unsigned uW, unsigned uH)
{
	(void)sizeof(NULL);
	return b1868_is_native(uW, uH);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_res_is_native_hint(unsigned uW, unsigned uH)
    __attribute__((alias("gj_deck_res_is_native_hint")));
