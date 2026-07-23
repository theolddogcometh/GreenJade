/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9899: milestone 9900 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_9900(void);
 *     - Returns the exclusive continuum wave id for the milestone 9900
 *       continuum (always 9900). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_9900  (alias)
 *   __libcgj_batch9899_marker = "libcgj-batch9899"
 *
 * Milestone 9900 exclusive continuum CREATE-ONLY (9891-9900). Unique
 * gj_continuum_wave_9900 surface only; no multi-def. Distinct from
 * gj_continuum_ready_9900 (batch9895), gj_continuum_wave_9800
 * (batch9799), gj_continuum_wave_9700 (batch9699), and
 * gj_continuum_wave_9600 (batch9599). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9899_marker[] = "libcgj-batch9899";

/* Exclusive continuum-wave id for milestone 9900. */
#define B9899_WAVE_ID  9900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9899_wave(void)
{
	return B9899_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_9900 - report the exclusive milestone-9900 wave id.
 *
 * Always returns 9900 (milestone 9900 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_9900(void)
{
	(void)NULL;
	return b9899_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_9900(void)
    __attribute__((alias("gj_continuum_wave_9900")));
