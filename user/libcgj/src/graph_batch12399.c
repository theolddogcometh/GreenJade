/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12399: milestone 12400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_12400(void);
 *     - Returns the exclusive continuum wave id for the milestone 12400
 *       continuum (always 12400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_12400  (alias)
 *   __libcgj_batch12399_marker = "libcgj-batch12399"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_continuum_wave_12400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_12400 (batch12395), gj_continuum_wave_12300
 * (batch12299), gj_continuum_wave_12200 (batch12199), and
 * gj_continuum_wave_12100 (batch12099). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12399_marker[] = "libcgj-batch12399";

/* Exclusive continuum-wave id for milestone 12400. */
#define B12399_WAVE_ID  12400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12399_wave(void)
{
	return B12399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_12400 - report the exclusive milestone-12400 wave id.
 *
 * Always returns 12400u (milestone 12400 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_12400(void)
{
	(void)NULL;
	return b12399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_12400(void)
    __attribute__((alias("gj_continuum_wave_12400")));
