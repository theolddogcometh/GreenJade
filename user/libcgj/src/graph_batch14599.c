/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14599: milestone 14600 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14600(void);
 *     - Returns the exclusive continuum wave id for the milestone 14600
 *       continuum (always 14600u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14600  (alias)
 *   __libcgj_batch14599_marker = "libcgj-batch14599"
 *
 * Milestone 14600 exclusive continuum CREATE-ONLY (14591-14600). Unique surface
 * only; no multi-def. Distinct from gj_*_14500 / gj_*_14400 / gj_*_14300 milestone
 * surfaces and sibling 14600 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14599_marker[] = "libcgj-batch14599";

/* Exclusive continuum-wave id for milestone 14600. */
#define B14599_WAVE_ID  14600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14599_wave(void)
{
	return B14599_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14600 - soft continuum surface
 *
 * Always returns 14600u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_wave_14600(void)
{
	(void)NULL;
	return b14599_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14600(void)
    __attribute__((alias("gj_continuum_wave_14600")));
