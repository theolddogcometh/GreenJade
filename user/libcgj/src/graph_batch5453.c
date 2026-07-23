/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5453: Steam Deck fan RPM stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_fan_rpm_5453(void);
 *     - Returns 0 (fan RPM stub = idle / unbound). Soft compile-time
 *       power/thermal product stub; not a live hwmon fan probe.
 *   uint32_t __gj_deck_fan_rpm_5453  (alias)
 *   __libcgj_batch5453_marker = "libcgj-batch5453"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_fan_rpm_5453 surface only;
 * no multi-def. Distinct from gj_deck_tdp_mw_5451 (batch5451) and
 * gj_deck_battery_pct_5452 (batch5452). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5453_marker[] = "libcgj-batch5453";

/* Fan RPM stub (idle / unbound probe). */
#define B5453_FAN_RPM  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5453_fan_rpm(void)
{
	return B5453_FAN_RPM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_fan_rpm_5453 - report Steam Deck fan RPM stub.
 *
 * Always returns 0 (idle / no probe). Soft pure-data thermal stub; does
 * not read hwmon or EC fan curves. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_fan_rpm_5453(void)
{
	(void)NULL;
	return b5453_fan_rpm();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_fan_rpm_5453(void)
    __attribute__((alias("gj_deck_fan_rpm_5453")));
