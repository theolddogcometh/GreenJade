/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3891: milestone 3900 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3900(void);
 *     - Returns the exclusive wave id for the milestone 3900 continuum
 *       (always 3900). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3900  (alias)
 *   __libcgj_batch3891_marker = "libcgj-batch3891"
 *
 * Milestone 3900 exclusive continuum CREATE-ONLY (3891-3900). Unique
 * gj_wave_id_3900 surface only; no multi-def. Distinct from
 * gj_wave_id_3700 (batch3691), gj_wave_id_3600 (batch3591),
 * gj_wave_id_3550 (batch3541), and gj_wave_id_3500 (batch3491).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3891_marker[] = "libcgj-batch3891";

/* Exclusive-wave id for milestone 3900 continuum. */
#define B3891_WAVE_ID  3900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3891_wave_id(void)
{
	return B3891_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3900 - report the exclusive milestone-3900 wave id.
 *
 * Always returns 3900 (milestone 3900 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3900(void)
{
	(void)NULL;
	return b3891_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3900(void)
    __attribute__((alias("gj_wave_id_3900")));
