/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4141: milestone 4150 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4150(void);
 *     - Returns the exclusive wave id for the milestone 4150 continuum
 *       (always 4150). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4150  (alias)
 *   __libcgj_batch4141_marker = "libcgj-batch4141"
 *
 * Milestone 4150 exclusive continuum CREATE-ONLY (4141-4150). Unique
 * gj_wave_id_4150 surface only; no multi-def. Distinct from
 * gj_wave_id_4100 (batch4091), gj_wave_id_4050 (batch4041), and
 * prior wave_id_* symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4141_marker[] = "libcgj-batch4141";

/* Exclusive-wave id for milestone 4150 continuum. */
#define B4141_WAVE_ID  4150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4141_wave_id(void)
{
	return B4141_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4150 - report the exclusive milestone-4150 wave id.
 *
 * Always returns 4150 (milestone 4150 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4150(void)
{
	(void)NULL;
	return b4141_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4150(void)
    __attribute__((alias("gj_wave_id_4150")));
