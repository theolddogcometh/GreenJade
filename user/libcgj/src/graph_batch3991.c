/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3991: milestone 4000 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4000(void);
 *     - Returns the exclusive wave id for the milestone 4000 continuum
 *       (always 4000). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4000  (alias)
 *   __libcgj_batch3991_marker = "libcgj-batch3991"
 *
 * Milestone 4000 exclusive continuum CREATE-ONLY (3991-4000). Unique
 * gj_wave_id_4000 surface only; no multi-def. Distinct from
 * gj_wave_id_3950 (batch3941), gj_wave_id_3900 (batch3891),
 * gj_wave_id_3700 (batch3691), and gj_wave_id_3600 (batch3591).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3991_marker[] = "libcgj-batch3991";

/* Exclusive-wave id for milestone 4000 continuum. */
#define B3991_WAVE_ID  4000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3991_wave_id(void)
{
	return B3991_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4000 - report the exclusive milestone-4000 wave id.
 *
 * Always returns 4000 (milestone 4000 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4000(void)
{
	(void)NULL;
	return b3991_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4000(void)
    __attribute__((alias("gj_wave_id_4000")));
