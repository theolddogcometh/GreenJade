/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13899: milestone 13900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13900(void);
 *     - Returns the exclusive continuum wave id for the milestone 13900
 *       continuum (always 13900u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13900  (alias)
 *   __libcgj_batch13899_marker = "libcgj-batch13899"
 *
 * Milestone 13900 exclusive continuum CREATE-ONLY (13891-13900). Unique
 * gj_continuum_wave_13900 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13900 (batch13895), gj_continuum_wave_13800
 * (batch13799), gj_continuum_wave_13700 (batch13699), and
 * gj_continuum_wave_13600 (batch13599). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13899_marker[] = "libcgj-batch13899";

/* Exclusive continuum-wave id for milestone 13900. */
#define B13899_WAVE_ID  13900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13899_wave(void)
{
	return B13899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13900 - report the exclusive milestone-13900 wave id.
 *
 * Always returns 13900u (milestone 13900 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13900(void)
{
	(void)NULL;
	return b13899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13900(void)
    __attribute__((alias("gj_continuum_wave_13900")));
