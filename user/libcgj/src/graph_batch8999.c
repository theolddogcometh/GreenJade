/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8999: milestone 9000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_9000(void);
 *     - Returns the exclusive continuum wave id for the milestone 9000
 *       continuum (always 9000). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_9000  (alias)
 *   __libcgj_batch8999_marker = "libcgj-batch8999"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_continuum_wave_9000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_9000 (batch8995), gj_continuum_wave_8900
 * (batch8899), gj_continuum_wave_8800 (batch8799), and
 * gj_continuum_wave_8700 (batch8699). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8999_marker[] = "libcgj-batch8999";

/* Exclusive continuum-wave id for milestone 9000. */
#define B8999_WAVE_ID  9000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8999_wave(void)
{
	return B8999_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_9000 - report the exclusive milestone-9000 wave id.
 *
 * Always returns 9000 (milestone 9000 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_9000(void)
{
	(void)NULL;
	return b8999_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_9000(void)
    __attribute__((alias("gj_continuum_wave_9000")));
