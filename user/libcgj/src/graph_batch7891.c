/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7891: milestone 7900 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7900(void);
 *     - Returns the exclusive wave id for the milestone 7900 continuum
 *       (always 7900). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7900  (alias)
 *   __libcgj_batch7891_marker = "libcgj-batch7891"
 *
 * Milestone 7900 exclusive continuum CREATE-ONLY (7891-7900). Unique
 * gj_wave_id_7900 surface only; no multi-def. Distinct from
 * gj_wave_id_7800 (batch7791), gj_wave_id_7700 (batch7691),
 * gj_wave_id_7600 (batch7591), and gj_wave_id_7500 (batch7491).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7891_marker[] = "libcgj-batch7891";

/* Exclusive-wave id for milestone 7900 continuum. */
#define B7891_WAVE_ID  7900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7891_wave_id(void)
{
	return B7891_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7900 - report the exclusive milestone-7900 wave id.
 *
 * Always returns 7900 (milestone 7900 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7900(void)
{
	(void)NULL;
	return b7891_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7900(void)
    __attribute__((alias("gj_wave_id_7900")));
