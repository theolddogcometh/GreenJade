/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13499: milestone 13500 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13500(void);
 *     - Returns the exclusive continuum wave id for the milestone 13500
 *       continuum (always 13500u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13500  (alias)
 *   __libcgj_batch13499_marker = "libcgj-batch13499"
 *
 * Milestone 13500 exclusive continuum CREATE-ONLY (13491-13500). Unique
 * gj_continuum_wave_13500 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13500 (batch13495), gj_continuum_wave_13400
 * (batch13399), gj_continuum_wave_13300 (batch13299), and
 * gj_continuum_wave_13200 (batch13199). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13499_marker[] = "libcgj-batch13499";

/* Exclusive continuum-wave id for milestone 13500. */
#define B13499_WAVE_ID  13500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13499_wave(void)
{
	return B13499_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13500 - report the exclusive milestone-13500 wave id.
 *
 * Always returns 13500u (milestone 13500 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13500(void)
{
	(void)NULL;
	return b13499_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13500(void)
    __attribute__((alias("gj_continuum_wave_13500")));
