/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10299: milestone 10300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_10300(void);
 *     - Returns the exclusive continuum wave id for the milestone 10300
 *       continuum (always 10300). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_10300  (alias)
 *   __libcgj_batch10299_marker = "libcgj-batch10299"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_continuum_wave_10300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_10300 (batch10295), gj_continuum_wave_10200
 * (batch10199), gj_continuum_wave_10100 (batch10099), and
 * gj_continuum_wave_10000 (batch9999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10299_marker[] = "libcgj-batch10299";

/* Exclusive continuum-wave id for milestone 10300. */
#define B10299_WAVE_ID  10300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10299_wave(void)
{
	return B10299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_10300 - report the exclusive milestone-10300 wave id.
 *
 * Always returns 10300 (milestone 10300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_10300(void)
{
	(void)NULL;
	return b10299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_10300(void)
    __attribute__((alias("gj_continuum_wave_10300")));
