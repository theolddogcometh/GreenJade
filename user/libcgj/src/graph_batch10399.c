/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10399: milestone 10400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_10400(void);
 *     - Returns the exclusive continuum wave id for the milestone 10400
 *       continuum (always 10400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_10400  (alias)
 *   __libcgj_batch10399_marker = "libcgj-batch10399"
 *
 * Milestone 10400 exclusive continuum CREATE-ONLY (10391-10400). Unique
 * gj_continuum_wave_10400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_10400 (batch10395), gj_continuum_wave_10300
 * (batch10299), gj_continuum_wave_10200 (batch10199), and
 * gj_continuum_wave_10100 (batch10099). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10399_marker[] = "libcgj-batch10399";

/* Exclusive continuum-wave id for milestone 10400. */
#define B10399_WAVE_ID  10400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10399_wave(void)
{
	return B10399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_10400 - report the exclusive milestone-10400 wave id.
 *
 * Always returns 10400u (milestone 10400 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_10400(void)
{
	(void)NULL;
	return b10399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_10400(void)
    __attribute__((alias("gj_continuum_wave_10400")));
