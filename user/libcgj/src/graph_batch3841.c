/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3841: milestone 3850 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3850(void);
 *     - Returns the exclusive wave id for the milestone 3850 continuum
 *       (always 3850). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3850  (alias)
 *   __libcgj_batch3841_marker = "libcgj-batch3841"
 *
 * Milestone 3850 exclusive continuum CREATE-ONLY (3841-3850). Unique
 * gj_wave_id_3850 surface only; no multi-def. Distinct from
 * gj_wave_id_3750 (batch3741), gj_wave_id_3700 (batch3691),
 * gj_wave_id_3600 (batch3591), and gj_wave_id_3550 (batch3541).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3841_marker[] = "libcgj-batch3841";

/* Exclusive-wave id for milestone 3850 continuum. */
#define B3841_WAVE_ID  3850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3841_wave_id(void)
{
	return B3841_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3850 - report the exclusive milestone-3850 wave id.
 *
 * Always returns 3850 (milestone 3850 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3850(void)
{
	(void)NULL;
	return b3841_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3850(void)
    __attribute__((alias("gj_wave_id_3850")));
