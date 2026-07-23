/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2691: milestone 2700 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_2700(void);
 *     - Returns the exclusive wave id for the milestone 2700 continuum
 *       (always 2700). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_2700  (alias)
 *   __libcgj_batch2691_marker = "libcgj-batch2691"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_wave_id_2700 surface only; no multi-def. Distinct from
 * gj_wave_id_2600 (batch2591), gj_wave_id_2500 (batch2491),
 * gj_wave_id_2400 (batch2391), and gj_wave_id_2300 (batch2291).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2691_marker[] = "libcgj-batch2691";

/* Exclusive-wave id for milestone 2700 continuum. */
#define B2691_WAVE_ID  2700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2691_wave_id(void)
{
	return B2691_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_2700 - report the exclusive milestone-2700 wave id.
 *
 * Always returns 2700 (milestone 2700 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 */
uint32_t
gj_wave_id_2700(void)
{
	(void)NULL;
	return b2691_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_2700(void)
    __attribute__((alias("gj_wave_id_2700")));
