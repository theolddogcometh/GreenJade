/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2991: milestone 3000 continuum wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_id_3000(void);
 *     - Returns the exclusive continuum wave id for the milestone 3000
 *       continuum (always 3000). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_id_3000  (alias)
 *   __libcgj_batch2991_marker = "libcgj-batch2991"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_continuum_wave_id_3000 surface only; no multi-def. Distinct from
 * gj_wave_id_2700 (batch2691), gj_wave_id_2600 (batch2591),
 * gj_wave_id_2500 (batch2491), and gj_product_wave_id (batch2191).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2991_marker[] = "libcgj-batch2991";

/* Exclusive continuum-wave id for milestone 3000. */
#define B2991_WAVE_ID  3000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2991_wave_id(void)
{
	return B2991_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_id_3000 - report the exclusive milestone-3000 wave id.
 *
 * Always returns 3000 (milestone 3000 continuum). Link-time presence of
 * this symbol tags the wave. Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_wave_id_3000(void)
{
	(void)NULL;
	return b2991_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_id_3000(void)
    __attribute__((alias("gj_continuum_wave_id_3000")));
