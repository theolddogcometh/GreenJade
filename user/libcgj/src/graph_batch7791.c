/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7791: milestone 7800 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7800(void);
 *     - Returns the exclusive wave id for the milestone 7800 continuum
 *       (always 7800). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7800  (alias)
 *   __libcgj_batch7791_marker = "libcgj-batch7791"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_wave_id_7800 surface only; no multi-def. Distinct from
 * gj_wave_id_7700 (batch7691), gj_wave_id_7600 (batch7591),
 * gj_wave_id_7500 (batch7491), and gj_wave_id_7400 (batch7391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7791_marker[] = "libcgj-batch7791";

/* Exclusive-wave id for milestone 7800 continuum. */
#define B7791_WAVE_ID  7800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7791_wave_id(void)
{
	return B7791_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7800 - report the exclusive milestone-7800 wave id.
 *
 * Always returns 7800 (milestone 7800 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7800(void)
{
	(void)NULL;
	return b7791_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7800(void)
    __attribute__((alias("gj_wave_id_7800")));
