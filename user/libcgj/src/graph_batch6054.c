/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6054: soft default critical thermal trip (°C).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_critical_c_6054(void);
 *     - Returns the soft default critical trip-point temperature in
 *       whole degrees Celsius (105). Pure-data stub; not a live ACPI
 *       thermal_zone or sysfs read.
 *   uint32_t __gj_trip_critical_c_6054  (alias)
 *   __libcgj_batch6054_marker = "libcgj-batch6054"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_critical_c_6054 surface only; no multi-def.
 * Distinct from gj_temp_c_is_critical_hint (batch1934) and
 * gj_trip_hot_c_6053. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6054_marker[] = "libcgj-batch6054";

/* Soft default critical trip point (°C). */
#define B6054_CRITICAL_C  105u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6054_critical_c(void)
{
	return B6054_CRITICAL_C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_critical_c_6054 - soft default critical trip temperature (°C).
 *
 * Always returns 105. Soft critical-trip continuum tag only. Does not
 * call libc or probe hardware. No parent wires.
 */
uint32_t
gj_trip_critical_c_6054(void)
{
	(void)NULL;
	return b6054_critical_c();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_critical_c_6054(void)
    __attribute__((alias("gj_trip_critical_c_6054")));
