/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9989: milestone 10000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_10000(void);
 *     - Returns the exclusive continuum wave id for the milestone 10000
 *       continuum (always 10000). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_10000  (alias)
 *   __libcgj_batch9989_marker = "libcgj-batch9989"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990). Unique
 * gj_continuum_wave_10000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_10000 (batch9985), gj_continuum_wave_9900
 * (batch9899), gj_continuum_wave_9800 (batch9799), and
 * gj_continuum_wave_9700 (batch9699). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9989_marker[] = "libcgj-batch9989";

/* Exclusive continuum-wave id for milestone 10000. */
#define B9989_WAVE_ID  10000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9989_wave(void)
{
	return B9989_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_10000 - report the exclusive milestone-10000 wave id.
 *
 * Always returns 10000 (milestone 10000 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_10000(void)
{
	(void)NULL;
	return b9989_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_10000(void)
    __attribute__((alias("gj_continuum_wave_10000")));
