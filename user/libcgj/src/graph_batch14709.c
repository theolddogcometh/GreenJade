/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14709: milestone 14725 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14709(void);
 *     - Returns the compile-time graph batch number for this TU (14709).
 *   uint32_t gj_continuum_wave_14709(void);
 *     - Returns the exclusive continuum wave id for the milestone 14725
 *       continuum (always 14725u). Soft compile-time product tag.
 *   uint32_t __gj_batch_id_14709  (alias)
 *   uint32_t __gj_continuum_wave_14709  (alias)
 *   __libcgj_batch14709_marker = "libcgj-batch14709"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 /
 * gj_*_14500 milestone surfaces and sibling 14725 milestone symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14709_marker[] = "libcgj-batch14709";

/* Exclusive continuum-wave id for milestone 14725. */
#define B14709_ID       14709u
#define B14709_WAVE_ID  14725u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14709_id(void)
{
	return B14709_ID;
}

static uint32_t
b14709_wave(void)
{
	return B14709_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14709 - report this TU's graph batch number.
 *
 * Always returns 14709.
 */
uint32_t
gj_batch_id_14709(void)
{
	(void)NULL;
	return b14709_id();
}

/*
 * gj_continuum_wave_14709 - soft continuum surface
 *
 * Always returns 14725u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_wave_14709(void)
{
	return b14709_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14709(void)
    __attribute__((alias("gj_batch_id_14709")));

uint32_t __gj_continuum_wave_14709(void)
    __attribute__((alias("gj_continuum_wave_14709")));
