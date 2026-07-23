/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5459: Steam Deck thermal-ok readiness lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_thermal_ok_5459(void);
 *     - Returns 1 (thermal path considered ok for this continuum). Soft
 *       compile-time power/thermal product status tag; not a live
 *       thermal_zone probe.
 *   uint32_t __gj_deck_thermal_ok_5459  (alias)
 *   __libcgj_batch5459_marker = "libcgj-batch5459"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_thermal_ok_5459 surface only;
 * no multi-def. Distinct from gj_deck_tdp_mw_5451 (batch5451) and
 * gj_deck_fan_rpm_5453 (batch5453). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5459_marker[] = "libcgj-batch5459";

/* Thermal-ok readiness lamp for this continuum. */
#define B5459_THERMAL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5459_thermal_ok(void)
{
	return B5459_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_thermal_ok_5459 - report Steam Deck thermal-ok readiness lamp.
 *
 * Always returns 1 (thermal path ok). Soft pure-data power/thermal
 * tag; does not read thermal_zone sysfs. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_deck_thermal_ok_5459(void)
{
	(void)NULL;
	return b5459_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_thermal_ok_5459(void)
    __attribute__((alias("gj_deck_thermal_ok_5459")));
