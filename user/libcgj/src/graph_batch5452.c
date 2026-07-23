/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5452: Steam Deck battery percent stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_battery_pct_5452(void);
 *     - Returns 100 (battery percent stub = full). Soft compile-time
 *       power/thermal product stub; not a live power_supply probe.
 *   uint32_t __gj_deck_battery_pct_5452  (alias)
 *   __libcgj_batch5452_marker = "libcgj-batch5452"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_battery_pct_5452 surface only;
 * no multi-def. Distinct from gj_deck_tdp_mw_5451 (batch5451). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5452_marker[] = "libcgj-batch5452";

/* Battery percent stub (full / unbound probe). */
#define B5452_BATTERY_PCT  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5452_battery_pct(void)
{
	return B5452_BATTERY_PCT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_battery_pct_5452 - report Steam Deck battery percent stub.
 *
 * Always returns 100. Soft pure-data power stub; does not read
 * power_supply sysfs or UEFI. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_battery_pct_5452(void)
{
	(void)NULL;
	return b5452_battery_pct();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_battery_pct_5452(void)
    __attribute__((alias("gj_deck_battery_pct_5452")));
