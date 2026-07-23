/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6053: soft default hot thermal trip (°C).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_hot_c_6053(void);
 *     - Returns the soft default hot trip-point temperature in whole
 *       degrees Celsius (95). Pure-data stub; not a live ACPI
 *       thermal_zone or sysfs read.
 *   uint32_t __gj_trip_hot_c_6053  (alias)
 *   __libcgj_batch6053_marker = "libcgj-batch6053"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_hot_c_6053 surface only; no multi-def.
 * Distinct from gj_temp_c_is_hot_hint (batch1935) and
 * gj_trip_passive_c_6051. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6053_marker[] = "libcgj-batch6053";

/* Soft default hot trip point (°C). */
#define B6053_HOT_C  95u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6053_hot_c(void)
{
	return B6053_HOT_C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_hot_c_6053 - soft default hot trip temperature (°C).
 *
 * Always returns 95. Soft hot-trip continuum tag only. Does not call
 * libc or probe hardware. No parent wires.
 */
uint32_t
gj_trip_hot_c_6053(void)
{
	(void)NULL;
	return b6053_hot_c();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_hot_c_6053(void)
    __attribute__((alias("gj_trip_hot_c_6053")));
