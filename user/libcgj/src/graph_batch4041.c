/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4041: milestone 4050 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4050(void);
 *     - Returns the exclusive wave id for the milestone 4050 continuum
 *       (always 4050). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4050  (alias)
 *   __libcgj_batch4041_marker = "libcgj-batch4041"
 *
 * Milestone 4050 exclusive continuum CREATE-ONLY (4041-4050). Unique
 * gj_wave_id_4050 surface only; no multi-def. Distinct from
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

const char __libcgj_batch4041_marker[] = "libcgj-batch4041";

/* Exclusive-wave id for milestone 4050 continuum. */
#define B4041_WAVE_ID  4050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4041_wave_id(void)
{
	return B4041_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4050 - report the exclusive milestone-4050 wave id.
 *
 * Always returns 4050 (milestone 4050 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4050(void)
{
	(void)NULL;
	return b4041_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4050(void)
    __attribute__((alias("gj_wave_id_4050")));
