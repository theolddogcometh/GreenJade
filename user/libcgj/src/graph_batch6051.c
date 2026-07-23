/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6051: soft default passive thermal trip (°C).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_passive_c_6051(void);
 *     - Returns the soft default passive trip-point temperature in
 *       whole degrees Celsius (90). Pure-data stub; not a live ACPI
 *       thermal_zone or sysfs read.
 *   uint32_t __gj_trip_passive_c_6051  (alias)
 *   __libcgj_batch6051_marker = "libcgj-batch6051"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs — passive_c, active0_c, hot_c, critical_c, type_is_critical,
 * type_is_hot, type_is_passive, crossed_u, hyst_apply_u,
 * trip_point_ready). Unique gj_trip_passive_c_6051 surface only; no
 * multi-def. Distinct from gj_temp_c_is_hot_hint (batch1935) and
 * gj_pwr_throttle_pct_u (batch5338). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6051_marker[] = "libcgj-batch6051";

/* Soft default passive trip point (°C). */
#define B6051_PASSIVE_C  90u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6051_passive_c(void)
{
	return B6051_PASSIVE_C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_passive_c_6051 - soft default passive trip temperature (°C).
 *
 * Always returns 90. Link-time presence tags the thermal trip continuum.
 * Does not call libc or probe hardware. No parent wires.
 */
uint32_t
gj_trip_passive_c_6051(void)
{
	(void)NULL;
	return b6051_passive_c();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_passive_c_6051(void)
    __attribute__((alias("gj_trip_passive_c_6051")));
