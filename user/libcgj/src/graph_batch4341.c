/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4341: milestone 4350 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4350(void);
 *     - Returns the exclusive wave id for the milestone 4350 continuum
 *       (always 4350). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4350  (alias)
 *   __libcgj_batch4341_marker = "libcgj-batch4341"
 *
 * Milestone 4350 exclusive continuum CREATE-ONLY (4341-4350). Unique
 * gj_wave_id_4350 surface only; no multi-def. Distinct from
 * gj_wave_id_4300 (batch4291), gj_wave_id_4100 (batch4091),
 * gj_wave_id_3950 (batch3941), and gj_wave_id_3900 (batch3891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4341_marker[] = "libcgj-batch4341";

/* Exclusive-wave id for milestone 4350 continuum. */
#define B4341_WAVE_ID  4350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4341_wave_id(void)
{
	return B4341_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4350 - report the exclusive milestone-4350 wave id.
 *
 * Always returns 4350 (milestone 4350 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4350(void)
{
	(void)NULL;
	return b4341_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4350(void)
    __attribute__((alias("gj_wave_id_4350")));
