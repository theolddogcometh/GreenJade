/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1931: battery percent clamp.
 *
 * Surface (unique symbols):
 *   int gj_battery_pct_clamp(int pct);
 *     — Clamp a battery percentage reading into the closed interval
 *       [0, 100]. Soft pure-data bound only; not a hardware sample.
 *   int __gj_battery_pct_clamp  (alias)
 *   __libcgj_batch1931_marker = "libcgj-batch1931"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1931_marker[] = "libcgj-batch1931";

/* Battery percent closed interval. */
#define B1931_PCT_MIN  0
#define B1931_PCT_MAX  100

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1931_pct_clamp(int nPct)
{
	if (nPct < B1931_PCT_MIN) {
		return B1931_PCT_MIN;
	}
	if (nPct > B1931_PCT_MAX) {
		return B1931_PCT_MAX;
	}
	return nPct;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_pct_clamp — confine pct to [0, 100].
 *
 * pct: raw battery percentage (may be negative or over 100)
 * Returns clamped percent in [0, 100].
 */
int
gj_battery_pct_clamp(int nPct)
{
	(void)NULL;
	return b1931_pct_clamp(nPct);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_battery_pct_clamp(int nPct)
    __attribute__((alias("gj_battery_pct_clamp")));
