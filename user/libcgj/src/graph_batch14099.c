/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14099: milestone 14100 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14100(void);
 *     - Returns the exclusive continuum wave id for the milestone 14100
 *       continuum (always 14100u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14100  (alias)
 *   __libcgj_batch14099_marker = "libcgj-batch14099"
 *
 * Milestone 14100 exclusive continuum CREATE-ONLY (14091-14100). Unique
 * gj_continuum_wave_14100 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14100 (batch14095), gj_continuum_wave_14000
 * (batch13999), gj_continuum_wave_13000 (batch12999), and
 * gj_continuum_wave_12900 (batch12899). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14099_marker[] = "libcgj-batch14099";

/* Exclusive continuum-wave id for milestone 14100. */
#define B14099_WAVE_ID  14100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14099_wave(void)
{
	return B14099_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14100 - report the exclusive milestone-14100 wave id.
 *
 * Always returns 14100u (milestone 14100 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_14100(void)
{
	(void)NULL;
	return b14099_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14100(void)
    __attribute__((alias("gj_continuum_wave_14100")));
