/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6591: milestone 6600 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6600(void);
 *     - Returns the exclusive wave id for the milestone 6600 continuum
 *       (always 6600). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6600  (alias)
 *   __libcgj_batch6591_marker = "libcgj-batch6591"
 *
 * Milestone 6600 exclusive continuum CREATE-ONLY (6591-6600). Unique
 * gj_wave_id_6600 surface only; no multi-def. Distinct from
 * gj_wave_id_6500 (batch6491), gj_wave_id_6400 (batch6391),
 * gj_wave_id_6300 (batch6291), and gj_wave_id_6200 (batch6191).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6591_marker[] = "libcgj-batch6591";

/* Exclusive-wave id for milestone 6600 continuum. */
#define B6591_WAVE_ID  6600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6591_wave_id(void)
{
	return B6591_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6600 - report the exclusive milestone-6600 wave id.
 *
 * Always returns 6600 (milestone 6600 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6600(void)
{
	(void)NULL;
	return b6591_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6600(void)
    __attribute__((alias("gj_wave_id_6600")));
