/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3291: milestone 3300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3300(void);
 *     - Returns the exclusive wave id for the milestone 3300 continuum
 *       (always 3300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3300  (alias)
 *   __libcgj_batch3291_marker = "libcgj-batch3291"
 *
 * Milestone 3300 exclusive continuum CREATE-ONLY (3291-3300). Unique
 * gj_wave_id_3300 surface only; no multi-def. Distinct from
 * gj_wave_id_3200 (batch3192), gj_wave_id_3100 (batch3093),
 * gj_continuum_wave_id_3000 (batch2991), and gj_wave_id_2700
 * (batch2691). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3291_marker[] = "libcgj-batch3291";

/* Exclusive-wave id for milestone 3300 continuum. */
#define B3291_WAVE_ID  3300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3291_wave_id(void)
{
	return B3291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3300 - report the exclusive milestone-3300 wave id.
 *
 * Always returns 3300 (milestone 3300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3300(void)
{
	(void)NULL;
	return b3291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3300(void)
    __attribute__((alias("gj_wave_id_3300")));
