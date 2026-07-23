/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12999: milestone 13000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_13000(void);
 *     - Returns the exclusive continuum wave id for the milestone 13000
 *       continuum (always 13000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_13000  (alias)
 *   __libcgj_batch12999_marker = "libcgj-batch12999"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_continuum_wave_13000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_13000 (batch12995), gj_continuum_wave_12900
 * (batch12899), gj_continuum_wave_12800 (batch12799), and
 * gj_continuum_wave_12700 (batch12699). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12999_marker[] = "libcgj-batch12999";

/* Exclusive continuum-wave id for milestone 13000. */
#define B12999_WAVE_ID  13000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12999_wave(void)
{
	return B12999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_13000 - report the exclusive milestone-13000 wave id.
 *
 * Always returns 13000u (milestone 13000 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_13000(void)
{
	(void)NULL;
	return b12999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_13000(void)
    __attribute__((alias("gj_continuum_wave_13000")));
