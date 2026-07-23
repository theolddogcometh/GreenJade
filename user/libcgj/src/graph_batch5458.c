/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5458: Steam Deck ambient light (lux) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_ambient_lux_5458(void);
 *     - Returns 0 (ambient lux stub = unbound / no sensor reading).
 *       Soft compile-time ambient-light product stub; not a live IIO
 *       illuminance probe.
 *   uint32_t __gj_deck_ambient_lux_5458  (alias)
 *   __libcgj_batch5458_marker = "libcgj-batch5458"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_ambient_lux_5458 surface only;
 * no multi-def. Distinct from gj_deck_brightness_pct_5457 (batch5457).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5458_marker[] = "libcgj-batch5458";

/* Ambient lux stub (unbound / no sensor). */
#define B5458_AMBIENT_LUX  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5458_ambient_lux(void)
{
	return B5458_AMBIENT_LUX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_ambient_lux_5458 - report Steam Deck ambient light lux stub.
 *
 * Always returns 0 (unbound). Soft pure-data ambient-light stub; does
 * not read IIO illuminance sensors. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_deck_ambient_lux_5458(void)
{
	(void)NULL;
	return b5458_ambient_lux();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_ambient_lux_5458(void)
    __attribute__((alias("gj_deck_ambient_lux_5458")));
