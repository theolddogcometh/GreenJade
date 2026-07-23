/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5885: Deck power-profile count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_power_profile_count_5885(void);
 *     - Returns 3 (Power Save / Balanced / Performance profile slots).
 *       Soft compile-time power-profile product stub; not a live
 *       gamescope or powertools probe.
 *   uint32_t __gj_deck_power_profile_count_5885  (alias)
 *   __libcgj_batch5885_marker = "libcgj-batch5885"
 *
 * Exclusive continuum CREATE-ONLY (5881-5890: power profile TDP steps
 * for Deck). Unique gj_deck_power_profile_count_5885 surface only; no
 * multi-def. Distinct from TDP step range stubs in 5881-5884.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5885_marker[] = "libcgj-batch5885";

/* Deck soft power-profile slot count (save / balanced / perf). */
#define B5885_POWER_PROFILE_COUNT  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5885_power_profile_count(void)
{
	return B5885_POWER_PROFILE_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_power_profile_count_5885 - report Deck power-profile slot count.
 *
 * Always returns 3. Soft pure-data power-profile stub; does not query
 * platform profiles or sysfs. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_power_profile_count_5885(void)
{
	(void)NULL;
	return b5885_power_profile_count();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_power_profile_count_5885(void)
    __attribute__((alias("gj_deck_power_profile_count_5885")));
