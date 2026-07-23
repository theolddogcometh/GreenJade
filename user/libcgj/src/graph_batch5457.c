/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5457: Steam Deck brightness percent stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_brightness_pct_5457(void);
 *     - Returns 50 (brightness percent stub = mid-scale). Soft
 *       compile-time display product stub; not a live backlight sysfs
 *       probe.
 *   uint32_t __gj_deck_brightness_pct_5457  (alias)
 *   __libcgj_batch5457_marker = "libcgj-batch5457"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_brightness_pct_5457 surface
 * only; no multi-def. Distinct from gj_deck_refresh_hz_5454
 * (batch5454). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5457_marker[] = "libcgj-batch5457";

/* Brightness percent stub (mid-scale). */
#define B5457_BRIGHTNESS_PCT  50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5457_brightness_pct(void)
{
	return B5457_BRIGHTNESS_PCT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_brightness_pct_5457 - report Steam Deck brightness percent stub.
 *
 * Always returns 50. Soft pure-data display stub; does not read
 * backlight sysfs. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_brightness_pct_5457(void)
{
	(void)NULL;
	return b5457_brightness_pct();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_brightness_pct_5457(void)
    __attribute__((alias("gj_deck_brightness_pct_5457")));
