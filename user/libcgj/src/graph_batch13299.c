/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13299: milestone 13300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13300(void);
 *     - Returns the exclusive continuum wave id for the milestone 13300
 *       continuum (always 13300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13300  (alias)
 *   __libcgj_batch13299_marker = "libcgj-batch13299"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_continuum_wave_13300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13300 (batch13295), gj_continuum_wave_13200
 * (batch13199), gj_continuum_wave_13100 (batch13099), and
 * gj_continuum_wave_13000 (batch12999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13299_marker[] = "libcgj-batch13299";

/* Exclusive continuum-wave id for milestone 13300. */
#define B13299_WAVE_ID  13300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13299_wave(void)
{
	return B13299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13300 - report the exclusive milestone-13300 wave id.
 *
 * Always returns 13300u (milestone 13300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13300(void)
{
	(void)NULL;
	return b13299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13300(void)
    __attribute__((alias("gj_continuum_wave_13300")));
