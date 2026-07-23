/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13799: milestone 13800 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13800(void);
 *     - Returns the exclusive continuum wave id for the milestone 13800
 *       continuum (always 13800u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13800  (alias)
 *   __libcgj_batch13799_marker = "libcgj-batch13799"
 *
 * Milestone 13800 exclusive continuum CREATE-ONLY (13791-13800). Unique
 * gj_continuum_wave_13800 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13800 (batch13795), gj_continuum_wave_13700
 * (batch13699), gj_continuum_wave_13600 (batch13599), and
 * gj_continuum_wave_13500 (batch13499). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13799_marker[] = "libcgj-batch13799";

/* Exclusive continuum-wave id for milestone 13800. */
#define B13799_WAVE_ID  13800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13799_wave(void)
{
	return B13799_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13800 - report the exclusive milestone-13800 wave id.
 *
 * Always returns 13800u (milestone 13800 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13800(void)
{
	(void)NULL;
	return b13799_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13800(void)
    __attribute__((alias("gj_continuum_wave_13800")));
