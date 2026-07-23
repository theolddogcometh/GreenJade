/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14199: milestone 14200 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_14200(void);
 *     - Returns the exclusive continuum wave id for the milestone 14200
 *       continuum (always 14200u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_14200  (alias)
 *   __libcgj_batch14199_marker = "libcgj-batch14199"
 *
 * Milestone 14200 exclusive continuum CREATE-ONLY (14191-14200). Unique
 * gj_continuum_wave_14200 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14200 (batch14195), gj_continuum_wave_14000
 * (batch13999), gj_continuum_wave_13000 (batch12999), and
 * gj_continuum_wave_12900 (batch12899). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14199_marker[] = "libcgj-batch14199";

/* Exclusive continuum-wave id for milestone 14200. */
#define B14199_WAVE_ID  14200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14199_wave(void)
{
	return B14199_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_14200 - report the exclusive milestone-14200 wave id.
 *
 * Always returns 14200u (milestone 14200 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_14200(void)
{
	(void)NULL;
	return b14199_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_14200(void)
    __attribute__((alias("gj_continuum_wave_14200")));
