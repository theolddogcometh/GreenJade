/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14399: milestone 14400 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14400(void);
 *     - Returns the exclusive continuum wave id for the milestone 14400
 *       continuum (always 14400u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14400  (alias)
 *   __libcgj_batch14399_marker = "libcgj-batch14399"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_continuum_wave_14400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14400 (batch14395), gj_continuum_wave_14000
 * (batch13999), gj_continuum_wave_13000 (batch12999), and
 * gj_continuum_wave_12900 (batch12899). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14399_marker[] = "libcgj-batch14399";

/* Exclusive continuum-wave id for milestone 14400. */
#define B14399_WAVE_ID  14400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14399_wave(void)
{
	return B14399_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14400 - report the exclusive milestone-14400 wave id.
 *
 * Always returns 14400u (milestone 14400 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_14400(void)
{
	(void)NULL;
	return b14399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14400(void)
    __attribute__((alias("gj_continuum_wave_14400")));
