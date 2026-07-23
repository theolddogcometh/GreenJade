/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6052: soft default active[0] thermal trip (°C).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_active0_c_6052(void);
 *     - Returns the soft default active[0] (first cooling-device) trip
 *       point temperature in whole degrees Celsius (55). Pure-data stub;
 *       not a live ACPI thermal_zone or sysfs read.
 *   uint32_t __gj_trip_active0_c_6052  (alias)
 *   __libcgj_batch6052_marker = "libcgj-batch6052"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_active0_c_6052 surface only; no multi-def.
 * Distinct from gj_trip_passive_c_6051 and gj_pwr_fan_curve_u
 * (batch5339). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6052_marker[] = "libcgj-batch6052";

/* Soft default active[0] trip point (°C). */
#define B6052_ACTIVE0_C  55u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6052_active0_c(void)
{
	return B6052_ACTIVE0_C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_active0_c_6052 - soft default active[0] trip temperature (°C).
 *
 * Always returns 55. Soft first-stage active cooling trip tag only.
 * Does not call libc or probe hardware. No parent wires.
 */
uint32_t
gj_trip_active0_c_6052(void)
{
	(void)NULL;
	return b6052_active0_c();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_active0_c_6052(void)
    __attribute__((alias("gj_trip_active0_c_6052")));
