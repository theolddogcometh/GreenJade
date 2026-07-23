/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6791: milestone 6800 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6800(void);
 *     - Returns the exclusive wave id for the milestone 6800 continuum
 *       (always 6800). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6800  (alias)
 *   __libcgj_batch6791_marker = "libcgj-batch6791"
 *
 * Milestone 6800 exclusive continuum CREATE-ONLY (6791-6800). Unique
 * gj_wave_id_6800 surface only; no multi-def. Distinct from
 * gj_wave_id_6700 (batch6691), gj_wave_id_6600 (batch6591),
 * gj_wave_id_6500 (batch6491), and gj_wave_id_6400 (batch6391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6791_marker[] = "libcgj-batch6791";

/* Exclusive-wave id for milestone 6800 continuum. */
#define B6791_WAVE_ID  6800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6791_wave_id(void)
{
	return B6791_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6800 - report the exclusive milestone-6800 wave id.
 *
 * Always returns 6800 (milestone 6800 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6800(void)
{
	(void)NULL;
	return b6791_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6800(void)
    __attribute__((alias("gj_wave_id_6800")));
