/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11299: milestone 11300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_11300(void);
 *     - Returns the exclusive continuum wave id for the milestone 11300
 *       continuum (always 11300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_11300  (alias)
 *   __libcgj_batch11299_marker = "libcgj-batch11299"
 *
 * Milestone 11300 exclusive continuum CREATE-ONLY (11291-11300). Unique
 * gj_continuum_wave_11300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_11300 (batch11295), gj_continuum_wave_11200
 * (batch11199), gj_continuum_wave_11100 (batch11099), and
 * gj_continuum_wave_11000 (batch10999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11299_marker[] = "libcgj-batch11299";

/* Exclusive continuum-wave id for milestone 11300. */
#define B11299_WAVE_ID  11300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11299_wave(void)
{
	return B11299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_11300 - report the exclusive milestone-11300 wave id.
 *
 * Always returns 11300u (milestone 11300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_11300(void)
{
	(void)NULL;
	return b11299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_11300(void)
    __attribute__((alias("gj_continuum_wave_11300")));
