/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12499: milestone 12500 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_12500(void);
 *     - Returns the exclusive continuum wave id for the milestone 12500
 *       continuum (always 12500u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_12500  (alias)
 *   __libcgj_batch12499_marker = "libcgj-batch12499"
 *
 * Milestone 12500 exclusive continuum CREATE-ONLY (12491-12500). Unique
 * gj_continuum_wave_12500 surface only; no multi-def. Distinct from
 * gj_continuum_ready_12500 (batch12495), gj_continuum_wave_12400
 * (batch12399), gj_continuum_wave_12300 (batch12299), and
 * gj_continuum_wave_12200 (batch12199). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12499_marker[] = "libcgj-batch12499";

/* Exclusive continuum-wave id for milestone 12500. */
#define B12499_WAVE_ID  12500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12499_wave(void)
{
	return B12499_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_12500 - report the exclusive milestone-12500 wave id.
 *
 * Always returns 12500u (milestone 12500 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_12500(void)
{
	(void)NULL;
	return b12499_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_12500(void)
    __attribute__((alias("gj_continuum_wave_12500")));
