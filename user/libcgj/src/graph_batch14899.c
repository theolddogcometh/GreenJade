/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14899: milestone 14900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14900(void);
 *     - Returns the exclusive continuum wave id for the milestone 14900
 *       continuum (always 14900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14900  (alias)
 *   __libcgj_batch14899_marker = "libcgj-batch14899"
 *
 * Milestone 14900 exclusive continuum CREATE-ONLY (14891-14900). Unique surface
 * only; no multi-def. Distinct from gj_*_14800 / gj_*_14700 / gj_*_14600 milestone
 * surfaces and sibling 14900 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14899_marker[] = "libcgj-batch14899";

/* Exclusive continuum-wave id for milestone 14900. */
#define B14899_WAVE_ID  14900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14899_wave(void)
{
	return B14899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14900 - soft continuum surface
 *
 * Always returns 14900u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_wave_14900(void)
{
	(void)NULL;
	return b14899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14900(void)
    __attribute__((alias("gj_continuum_wave_14900")));
