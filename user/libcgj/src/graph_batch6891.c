/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6891: milestone 6900 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6900(void);
 *     - Returns the exclusive wave id for the milestone 6900 continuum
 *       (always 6900). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6900  (alias)
 *   __libcgj_batch6891_marker = "libcgj-batch6891"
 *
 * Milestone 6900 exclusive continuum CREATE-ONLY (6891-6900). Unique
 * gj_wave_id_6900 surface only; no multi-def. Distinct from
 * gj_wave_id_6800 (batch6791), gj_wave_id_6700 (batch6691),
 * gj_wave_id_6600 (batch6591), and gj_wave_id_6500 (batch6491).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6891_marker[] = "libcgj-batch6891";

/* Exclusive-wave id for milestone 6900 continuum. */
#define B6891_WAVE_ID  6900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6891_wave_id(void)
{
	return B6891_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6900 - report the exclusive milestone-6900 wave id.
 *
 * Always returns 6900 (milestone 6900 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6900(void)
{
	(void)NULL;
	return b6891_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6900(void)
    __attribute__((alias("gj_wave_id_6900")));
