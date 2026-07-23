/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8899: milestone 8900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_8900(void);
 *     - Returns the exclusive continuum wave id for the milestone 8900
 *       continuum (always 8900). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_8900  (alias)
 *   __libcgj_batch8899_marker = "libcgj-batch8899"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_continuum_wave_8900 surface only; no multi-def. Distinct from
 * gj_continuum_ready_8900 (batch8895), gj_continuum_wave_8800
 * (batch8799), gj_continuum_wave_8700 (batch8699), and
 * gj_continuum_wave_8600 (batch8599). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8899_marker[] = "libcgj-batch8899";

/* Exclusive continuum-wave id for milestone 8900. */
#define B8899_WAVE_ID  8900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8899_wave(void)
{
	return B8899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_8900 - report the exclusive milestone-8900 wave id.
 *
 * Always returns 8900 (milestone 8900 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_8900(void)
{
	(void)NULL;
	return b8899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_8900(void)
    __attribute__((alias("gj_continuum_wave_8900")));
