/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6060: thermal trip continuum ready + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_point_ready_6060(void);
 *     - Returns 1 (thermal trip point continuum ready lamp for the
 *       6051-6060 exclusive wave). Soft compile-time product tag.
 *   uint32_t gj_batch_id_6060(void);
 *     - Returns the compile-time graph batch number for this TU (6060).
 *   uint32_t __gj_trip_point_ready_6060  (alias)
 *   uint32_t __gj_batch_id_6060  (alias)
 *   __libcgj_batch6060_marker = "libcgj-batch6060"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs — passive_c, active0_c, hot_c, critical_c, type_is_critical,
 * type_is_hot, type_is_passive, crossed_u, hyst_apply_u,
 * trip_point_ready). Unique *_6060 surfaces only; no multi-def.
 * Distinct from prior gj_batch_id_* and thermal readiness lamps.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6060_marker[] = "libcgj-batch6060";

#define B6060_BATCH_ID  6060u
#define B6060_READY     1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6060_ready(void)
{
	return B6060_READY;
}

static uint32_t
b6060_id(void)
{
	return B6060_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_point_ready_6060 - thermal trip continuum readiness lamp.
 *
 * Always returns 1 (wave 6051-6060 present/ready). Does not call libc
 * or probe hardware. No parent wires.
 */
uint32_t
gj_trip_point_ready_6060(void)
{
	(void)NULL;
	return b6060_ready();
}

/*
 * gj_batch_id_6060 - report this TU's graph batch number.
 *
 * Always returns 6060.
 */
uint32_t
gj_batch_id_6060(void)
{
	return b6060_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_trip_point_ready_6060(void)
    __attribute__((alias("gj_trip_point_ready_6060")));

uint32_t __gj_batch_id_6060(void)
    __attribute__((alias("gj_batch_id_6060")));
