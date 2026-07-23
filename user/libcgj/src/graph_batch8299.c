/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8299: milestone 8300 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_8300(void);
 *     - Returns the exclusive continuum wave id for the milestone 8300
 *       continuum (always 8300). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_8300  (alias)
 *   __libcgj_batch8299_marker = "libcgj-batch8299"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_continuum_wave_8300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_8300 (batch8295), gj_wave_id_8200 (batch8191),
 * gj_continuum_wave_id_3000 (batch2991), and gj_continuum_wave_ok
 * (batch2599). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8299_marker[] = "libcgj-batch8299";

/* Exclusive continuum-wave id for milestone 8300. */
#define B8299_WAVE_ID  8300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8299_wave(void)
{
	return B8299_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_8300 - report the exclusive milestone-8300 wave id.
 *
 * Always returns 8300 (milestone 8300 continuum wave). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_continuum_wave_8300(void)
{
	(void)NULL;
	return b8299_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_8300(void)
    __attribute__((alias("gj_continuum_wave_8300")));
