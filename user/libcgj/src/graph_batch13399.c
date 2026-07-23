/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13399: milestone 13400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13400(void);
 *     - Returns the exclusive continuum wave id for the milestone 13400
 *       continuum (always 13400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13400  (alias)
 *   __libcgj_batch13399_marker = "libcgj-batch13399"
 *
 * Milestone 13400 exclusive continuum CREATE-ONLY (13391-13400). Unique
 * gj_continuum_wave_13400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13400 (batch13395), gj_continuum_wave_13300
 * (batch13299), gj_continuum_wave_13200 (batch13199), and
 * gj_continuum_wave_13100 (batch13099). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13399_marker[] = "libcgj-batch13399";

/* Exclusive continuum-wave id for milestone 13400. */
#define B13399_WAVE_ID  13400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13399_wave(void)
{
	return B13399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13400 - report the exclusive milestone-13400 wave id.
 *
 * Always returns 13400u (milestone 13400 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13400(void)
{
	(void)NULL;
	return b13399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13400(void)
    __attribute__((alias("gj_continuum_wave_13400")));
