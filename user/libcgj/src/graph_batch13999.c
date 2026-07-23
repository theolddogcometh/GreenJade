/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13999: milestone 14000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14000(void);
 *     - Returns the exclusive continuum wave id for the milestone 14000
 *       continuum (always 14000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14000  (alias)
 *   __libcgj_batch13999_marker = "libcgj-batch13999"
 *
 * Milestone 14000 exclusive continuum CREATE-ONLY (13991-14000). Unique
 * gj_continuum_wave_14000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14000 (batch13995), gj_continuum_wave_13000
 * (batch12999), gj_continuum_wave_12900 (batch12899), and
 * gj_continuum_wave_12800 (batch12799). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13999_marker[] = "libcgj-batch13999";

/* Exclusive continuum-wave id for milestone 14000. */
#define B13999_WAVE_ID  14000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13999_wave(void)
{
	return B13999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14000 - report the exclusive milestone-14000 wave id.
 *
 * Always returns 14000u (milestone 14000 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_14000(void)
{
	(void)NULL;
	return b13999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14000(void)
    __attribute__((alias("gj_continuum_wave_14000")));
