/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6359: Xbox pad battery percent validity stub.
 *
 * Surface (unique symbols):
 *   int gj_xpad_battery_ok_6359(uint32_t percent);
 *     - Return 1 if percent is a plausible wireless-pad charge reading
 *       in [0, 100]; else 0. Soft gate before battery UI / power policy.
 *   int __gj_xpad_battery_ok_6359  (alias)
 *   __libcgj_batch6359_marker = "libcgj-batch6359"
 *
 * Exclusive continuum CREATE-ONLY (6351-6360: Xbox controller feature
 * stubs). Unique gj_xpad_battery_ok_6359 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6359_marker[] = "libcgj-batch6359";

#define B6359_PCT_MIN  0u
#define B6359_PCT_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6359_ok(uint32_t uPct)
{
	if (uPct > B6359_PCT_MAX) {
		return 0;
	}
	(void)B6359_PCT_MIN;
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_battery_ok_6359 - 1 if percent is in [0, 100].
 *
 * Pure predicate for Xbox wireless / Series pad charge samples.
 * Does not call libc. No parent wires.
 */
int
gj_xpad_battery_ok_6359(uint32_t uPercent)
{
	(void)NULL;
	return b6359_ok(uPercent);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xpad_battery_ok_6359(uint32_t uPercent)
    __attribute__((alias("gj_xpad_battery_ok_6359")));
