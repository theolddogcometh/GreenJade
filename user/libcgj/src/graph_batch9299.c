/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9299: milestone 9300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_9300(void);
 *     - Returns the exclusive continuum wave id for the milestone 9300
 *       continuum (always 9300). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_9300  (alias)
 *   __libcgj_batch9299_marker = "libcgj-batch9299"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_continuum_wave_9300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_9300 (batch9295), gj_continuum_wave_9200
 * (batch9199), gj_continuum_wave_9100 (batch9099), and
 * gj_continuum_wave_9000 (batch8999). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9299_marker[] = "libcgj-batch9299";

/* Exclusive continuum-wave id for milestone 9300. */
#define B9299_WAVE_ID  9300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9299_wave(void)
{
	return B9299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_9300 - report the exclusive milestone-9300 wave id.
 *
 * Always returns 9300 (milestone 9300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_9300(void)
{
	(void)NULL;
	return b9299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_9300(void)
    __attribute__((alias("gj_continuum_wave_9300")));
