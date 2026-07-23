/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12299: milestone 12300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_12300(void);
 *     - Returns the exclusive continuum wave id for the milestone 12300
 *       continuum (always 12300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_12300  (alias)
 *   __libcgj_batch12299_marker = "libcgj-batch12299"
 *
 * Milestone 12300 exclusive continuum CREATE-ONLY (12291-12300). Unique
 * gj_continuum_wave_12300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_12300 (batch12295), gj_continuum_wave_12200
 * (batch12199), gj_continuum_wave_12100 (batch12099), and
 * gj_continuum_wave_12000 (batch11999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12299_marker[] = "libcgj-batch12299";

/* Exclusive continuum-wave id for milestone 12300. */
#define B12299_WAVE_ID  12300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12299_wave(void)
{
	return B12299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_12300 - report the exclusive milestone-12300 wave id.
 *
 * Always returns 12300u (milestone 12300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_12300(void)
{
	(void)NULL;
	return b12299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_12300(void)
    __attribute__((alias("gj_continuum_wave_12300")));
