/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3093: milestone 3100 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3100(void);
 *     - Returns the exclusive wave id for the milestone 3100 continuum
 *       (always 3100). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3100  (alias)
 *   __libcgj_batch3093_marker = "libcgj-batch3093"
 *
 * Milestone 3100 exclusive continuum CREATE-ONLY (3091-3100). Unique
 * gj_wave_id_3100 surface only; no multi-def. Distinct from
 * gj_continuum_wave_id_3000 (batch2991), gj_wave_id_2700 (batch2691),
 * gj_wave_id_2600 (batch2591), and gj_wave_id_2500 (batch2491).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3093_marker[] = "libcgj-batch3093";

/* Exclusive-wave id for milestone 3100 continuum. */
#define B3093_WAVE_ID  3100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3093_wave_id(void)
{
	return B3093_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3100 - report the exclusive milestone-3100 wave id.
 *
 * Always returns 3100 (milestone 3100 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3100(void)
{
	(void)NULL;
	return b3093_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3100(void)
    __attribute__((alias("gj_wave_id_3100")));
