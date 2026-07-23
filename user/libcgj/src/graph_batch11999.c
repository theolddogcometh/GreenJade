/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11999: milestone 12000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_12000(void);
 *     - Returns the exclusive continuum wave id for the milestone 12000
 *       continuum (always 12000u). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_12000  (alias)
 *   __libcgj_batch11999_marker = "libcgj-batch11999"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_continuum_wave_12000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_12000 (batch11995), gj_continuum_wave_11900
 * (batch11899), gj_continuum_wave_11800 (batch11799), and
 * gj_continuum_wave_11700 (batch11699). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11999_marker[] = "libcgj-batch11999";

/* Exclusive continuum-wave id for milestone 12000. */
#define B11999_WAVE_ID  12000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11999_wave(void)
{
	return B11999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_12000 - report the exclusive milestone-12000 wave id.
 *
 * Always returns 12000u (milestone 12000 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_12000(void)
{
	(void)NULL;
	return b11999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_12000(void)
    __attribute__((alias("gj_continuum_wave_12000")));
