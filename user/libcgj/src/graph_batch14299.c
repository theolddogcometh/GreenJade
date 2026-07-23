/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14299: milestone 14300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14300(void);
 *     - Returns the exclusive continuum wave id for the milestone 14300
 *       continuum (always 14300u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14300  (alias)
 *   __libcgj_batch14299_marker = "libcgj-batch14299"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_continuum_wave_14300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14300 (batch14295), gj_continuum_wave_14200
 * (batch14199), gj_continuum_wave_14100 (batch14099), and
 * gj_continuum_wave_14000 (batch13999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14299_marker[] = "libcgj-batch14299";

/* Exclusive continuum-wave id for milestone 14300. */
#define B14299_WAVE_ID  14300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14299_wave(void)
{
	return B14299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14300 - report the exclusive milestone-14300 wave id.
 *
 * Always returns 14300u (milestone 14300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_14300(void)
{
	(void)NULL;
	return b14299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14300(void)
    __attribute__((alias("gj_continuum_wave_14300")));
